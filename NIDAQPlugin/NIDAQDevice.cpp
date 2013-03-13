//
//  NIDAQDevice.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQDevice.h"

#include <cstring>
#include <spawn.h>
#include <sys/wait.h>

#include <openssl/rand.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>


BEGIN_NAMESPACE_MW


const std::string NIDAQDevice::NAME("name");
const std::string NIDAQDevice::UPDATE_INTERVAL("update_interval");
const std::string NIDAQDevice::ANALOG_INPUT_DATA_INTERVAL("analog_input_data_interval");
const std::string NIDAQDevice::ANALOG_OUTPUT_DATA_INTERVAL("analog_output_data_interval");
const std::string NIDAQDevice::ASSUME_MULTIPLEXED_ADC("assume_multiplexed_adc");


void NIDAQDevice::describeComponent(ComponentInfo &info) {
    IODevice::describeComponent(info);
    
    info.setSignature("iodevice/nidaq");
    
    info.addParameter(NAME, true, "Dev1");
    info.addParameter(UPDATE_INTERVAL, true, "3ms");
    info.addParameter(ANALOG_INPUT_DATA_INTERVAL, true, "1ms");
    info.addParameter(ANALOG_OUTPUT_DATA_INTERVAL, true, "1ms");
    info.addParameter(ASSUME_MULTIPLEXED_ADC, "1");
}


static std::string generateUniqueID() {
    boost::uint64_t uniqueID;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    int status = RAND_pseudo_bytes(reinterpret_cast<unsigned char *>(&uniqueID), sizeof(uniqueID));
#pragma clang diagnostic pop
    if (status < 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                              "Internal error: Unable to generate unique identifier for NIDAQ device IPC resource");
    }
    return (boost::format("%x") % uniqueID).str();
}


NIDAQDevice::NIDAQDevice(const ParameterValueMap &parameters) :
    IODevice(parameters),
    deviceName(parameters[NAME].str()),
    requestSemName(generateUniqueID()),
    responseSemName(generateUniqueID()),
    controlChannel(boost::interprocess::create_only, requestSemName, responseSemName),
    sharedMemoryName(generateUniqueID()),
    sharedMemory(boost::interprocess::create_only, sharedMemoryName),
    controlMessage(NULL),
    helperPID(-1),
    updateInterval(parameters[UPDATE_INTERVAL]),
    analogInputDataInterval(parameters[ANALOG_INPUT_DATA_INTERVAL]),
    assumeMultiplexedADC(parameters[ASSUME_MULTIPLEXED_ADC]),
    analogInputSampleBufferSize(0),
    analogInputTaskRunning(false),
    analogOutputDataInterval(parameters[ANALOG_OUTPUT_DATA_INTERVAL]),
    analogOutputSampleBufferSize(0),
    analogOutputTaskRunning(false)
{
    if (updateInterval <= 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid update interval");
    }
    
    if (analogInputDataInterval <= 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid analog input data interval");
    }
    if (updateInterval % analogInputDataInterval != 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                              "Update interval must be an integer multiple of analog input data interval");
    }
    
    if (analogOutputDataInterval <= 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid analog output data interval");
    }
}


void NIDAQDevice::addChild(std::map<std::string, std::string> parameters,
                           ComponentRegistryPtr reg,
                           boost::shared_ptr<Component> child)
{
    boost::shared_ptr<NIDAQAnalogInputVoltageChannel> aiChannel = boost::dynamic_pointer_cast<NIDAQAnalogInputVoltageChannel>(child);
    if (aiChannel) {
        analogInputChannels.push_back(aiChannel);
        return;
    }
    
    boost::shared_ptr<NIDAQAnalogOutputVoltageWaveformChannel> aoChannel = boost::dynamic_pointer_cast<NIDAQAnalogOutputVoltageWaveformChannel>(child);
    if (aoChannel) {
        MWTime period = aoChannel->getPeriod();
        if (period % analogOutputDataInterval != 0) {
            throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                                  "Analog output waveform period must be an integer multiple of analog output data interval");
        }
        
        // This is the per-channel buffer size.  We'll multiply by the number of channels in initialize().
        analogOutputSampleBufferSize = std::max(analogOutputSampleBufferSize,
                                                size_t(period / analogOutputDataInterval));
        
        analogOutputChannels.push_back(aoChannel);
        return;
    }
    
    throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid channel type for NIDAQ device");
}


bool NIDAQDevice::initialize() {
    if (analogInputChannels.empty() && analogOutputChannels.empty()) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "NIDAQ device must have at least one channel");
    }
    
    analogInputSampleBufferSize = (size_t(updateInterval / analogInputDataInterval) *
                                   analogInputChannels.size());
    analogOutputSampleBufferSize *= analogOutputChannels.size();
    
    sharedMemory.setSize(HelperControlMessage::sizeWithSamples(std::max(analogInputSampleBufferSize,
                                                                        analogOutputSampleBufferSize)));
    controlMessage = sharedMemory.getMessagePtr();
    
    spawnHelper();
    
    mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Configuring NIDAQ device \"%s\"...", deviceName.c_str());
    
    if (haveAnalogInputChannels() && !createAnalogInputTask()) {
        return false;
    }
    if (haveAnalogOutputChannels() && !createAnalogOutputTask()) {
        return false;
    }
    
    mprintf(M_IODEVICE_MESSAGE_DOMAIN, "NIDAQ device \"%s\" is ready", deviceName.c_str());
    
    return true;
}


bool NIDAQDevice::createAnalogInputTask() {
    BOOST_FOREACH(boost::shared_ptr<NIDAQAnalogInputVoltageChannel> channel, analogInputChannels) {
        controlMessage->code = HelperControlMessage::REQUEST_CREATE_ANALOG_INPUT_VOLTAGE_CHANNEL;
        
        controlMessage->analogVoltageChannel.channelNumber = channel->getChannelNumber();
        controlMessage->analogVoltageChannel.minVal = channel->getRangeMin();
        controlMessage->analogVoltageChannel.maxVal = channel->getRangeMax();
        
        if (!sendHelperRequest()) {
            return false;
        }
    }
    
    controlMessage->code = HelperControlMessage::REQUEST_SET_ANALOG_INPUT_SAMPLE_CLOCK_TIMING;
    controlMessage->sampleClockTiming.samplingRate = 1.0 / (analogInputDataInterval / 1e6);  // us to s
    controlMessage->sampleClockTiming.samplesPerChannelToAcquire = (updateInterval /
                                                                    analogInputDataInterval);
    if (!sendHelperRequest()) {
        return false;
    }
    
    return true;
}


bool NIDAQDevice::createAnalogOutputTask() {
    BOOST_FOREACH(boost::shared_ptr<NIDAQAnalogOutputVoltageWaveformChannel> channel, analogOutputChannels) {
        controlMessage->code = HelperControlMessage::REQUEST_CREATE_ANALOG_OUTPUT_VOLTAGE_CHANNEL;
        
        controlMessage->analogVoltageChannel.channelNumber = channel->getChannelNumber();
        controlMessage->analogVoltageChannel.minVal = channel->getRangeMin();
        controlMessage->analogVoltageChannel.maxVal = channel->getRangeMax();
        
        if (!sendHelperRequest()) {
            return false;
        }
    }
    
    controlMessage->code = HelperControlMessage::REQUEST_SET_ANALOG_OUTPUT_SAMPLE_CLOCK_TIMING;
    controlMessage->sampleClockTiming.samplingRate = 1.0 / (analogOutputDataInterval / 1e6);  // us to s
    controlMessage->sampleClockTiming.samplesPerChannelToAcquire = (analogOutputSampleBufferSize /
                                                                    analogOutputChannels.size());
    if (!sendHelperRequest()) {
        return false;
    }
    
    return true;
}


bool NIDAQDevice::startDeviceIO() {
    scoped_lock lock(controlMutex);
    
    if (haveAnalogOutputChannels() && !startAnalogOutputTask()) {
        return false;
    }
    if (haveAnalogInputChannels() && !startAnalogInputTask()) {
        return false;
    }
    
    return true;
}


bool NIDAQDevice::startAnalogOutputTask() {
    if (analogOutputTaskRunning) {
        return true;
    }
    
    const size_t numChannels = analogOutputChannels.size();
    
    controlMessage->code = HelperControlMessage::REQUEST_WRITE_ANALOG_OUTPUT_SAMPLES;
    controlMessage->analogSamples.timeout = 10.0;
    controlMessage->analogSamples.samples.numSamples = analogOutputSampleBufferSize;
    
    for (size_t i = 0; i < analogOutputSampleBufferSize; i++) {
        double &sample = controlMessage->analogSamples.samples[i];
        MWTime sampleTime = analogOutputDataInterval * (i / numChannels);
        sample = analogOutputChannels[i % numChannels]->getSampleForTime(sampleTime);
    }
    
    if (!sendHelperRequest()) {
        return false;
    }
    
    const size_t numSamplesWritten = controlMessage->analogSamples.samples.numSamples;
    
    if (numSamplesWritten != analogOutputSampleBufferSize) {
        merror(M_IODEVICE_MESSAGE_DOMAIN,
               "Wrote only %u of %u analog output samples to NIDAQ device",
               numSamplesWritten,
               analogOutputSampleBufferSize);
        return false;
    }
    
    controlMessage->code = HelperControlMessage::REQUEST_START_ANALOG_OUTPUT_TASK;
    if (!sendHelperRequest()) {
        return false;
    }
    
    analogOutputTaskRunning = true;
    
    return true;
}


bool NIDAQDevice::startAnalogInputTask() {
    if (!analogInputTaskRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_START_ANALOG_INPUT_TASK;
        if (!sendHelperRequest()) {
            return false;
        }
        
        HelperControlMessage::unsigned_int systemBaseTimeNS = Clock::instance()->getSystemBaseTimeNS();
        analogInputStartTime = MWTime((controlMessage->taskStartTime - systemBaseTimeNS)
                                      / HelperControlMessage::unsigned_int(1000));  // ns to us
        totalNumAnalogInputSamplesAcquired = 0;
        
        analogInputTaskRunning = true;
    }
    
    if (!analogInputScheduleTask) {
        boost::shared_ptr<NIDAQDevice> sharedThis = component_shared_from_this<NIDAQDevice>();
        analogInputScheduleTask = Scheduler::instance()->scheduleUS(FILELINE,
                                                                    0,
                                                                    updateInterval,
                                                                    M_REPEAT_INDEFINITELY,
                                                                    boost::bind(&NIDAQDevice::readAnalogInput, sharedThis),
                                                                    M_DEFAULT_IODEVICE_PRIORITY,
                                                                    M_DEFAULT_IODEVICE_WARN_SLOP_US,
                                                                    M_DEFAULT_IODEVICE_FAIL_SLOP_US,
                                                                    M_MISSED_EXECUTION_DROP);
    }
    
    return true;
}


bool NIDAQDevice::stopDeviceIO() {
    scoped_lock lock(controlMutex);
    
    bool success = true;
    
    if (analogInputScheduleTask) {
        analogInputScheduleTask->cancel();
        analogInputScheduleTask.reset();
    }
    
    if (analogInputTaskRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_STOP_ANALOG_INPUT_TASK;
        if (!sendHelperRequest()) {
            success = false;
        } else {
            analogInputTaskRunning = false;
        }
    }
    
    if (analogOutputTaskRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_STOP_ANALOG_OUTPUT_TASK;
        if (!sendHelperRequest()) {
            success = false;
        } else {
            analogOutputTaskRunning = false;
        }
    }
    
    return success;
}


void* NIDAQDevice::readAnalogInput() {
    scoped_lock lock(controlMutex);
    
    if (!analogInputScheduleTask) {
        // If we've already been canceled, don't try to read more data
        return NULL;
    }
    
    controlMessage->code = HelperControlMessage::REQUEST_READ_ANALOG_INPUT_SAMPLES;
    controlMessage->analogSamples.timeout = double(updateInterval) / 1e6;  // us to s
    controlMessage->analogSamples.samples.numSamples = analogInputSampleBufferSize;
    
    if (!sendHelperRequest()) {
        return NULL;
    }
    
    const size_t numSamplesRead = controlMessage->analogSamples.samples.numSamples;
    const size_t numChannels = analogInputChannels.size();
    const MWTime firstSampleTime = analogInputStartTime + (totalNumAnalogInputSamplesAcquired / numChannels *
                                                           analogInputDataInterval);
    
    // Do this here in case there's an exception while posting the samples
    totalNumAnalogInputSamplesAcquired += numSamplesRead;
    
    if (numSamplesRead != analogInputSampleBufferSize) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
                 "NIDAQ device requested %u analog samples but got only %u",
                 analogInputSampleBufferSize,
                 numSamplesRead);
    }
    
    for (size_t i = 0; i < numSamplesRead; i++) {
        double sample = controlMessage->analogSamples.samples[i];
        MWTime sampleTime = firstSampleTime;
        
        //
        // analogInputDataInterval determines the *per-channel* sampling rate.  The method for computing a
        // timestamp for each sample depends on whether the device supports simultaneous sampling, i.e. whether
        // there's a separate analog-to-digital converter (ADC) for each channel, or there's a single ADC that
        // is multiplexed among all channels.  Given an acquisition with N analog input channels and a per-channel
        // sampling rate of S samples per second:
        //
        if (assumeMultiplexedADC) {
            // If there's a single, multiplexed ADC, then the true sampling rate of the device is N*S, and each sample
            // is acquired 1/(N*S) seconds (i.e. analogInputDataInterval/numChannels microseconds) after the previous
            // sample
            sampleTime += ((analogInputDataInterval * i)  // Multiply first to minimize truncation errors
                           / numChannels);
        } else {
            // If there's a separate ADC for each channel, then the device acquires N simultaneous samples every 1/S
            // seconds, and all N samples get the same timestamp
            sampleTime += analogInputDataInterval * (i / numChannels);
        }
        
        analogInputChannels[i % numChannels]->postSample(sample, sampleTime);
    }
    
    return NULL;
}


NIDAQDevice::~NIDAQDevice() {
    reapHelper();
}


void NIDAQDevice::spawnHelper() {
    const char * const argv[] = {
        PLUGIN_HELPER_EXECUTABLE,
        requestSemName.c_str(),
        responseSemName.c_str(),
        sharedMemoryName.c_str(),
        deviceName.c_str(),
        0
    };
    
    int status = posix_spawn(&helperPID,
                             PLUGIN_HELPERS_DIR "/" PLUGIN_HELPER_EXECUTABLE,
                             NULL,
                             NULL,
                             const_cast<char * const *>(argv),
                             NULL);
    
    if (status != 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                              "Unable to launch " PLUGIN_HELPER_EXECUTABLE,
                              std::strerror(status));
    }
}


void NIDAQDevice::reapHelper() {
    if (helperPID <= 0) {
        return;
    }
    
    controlMessage->code = HelperControlMessage::REQUEST_SHUTDOWN;
    sendHelperRequest();
    
    int status;
    if (-1 == waitpid(helperPID, &status, WUNTRACED)) {
        merror(M_IODEVICE_MESSAGE_DOMAIN,
               "Error while waiting for %s to exit: %s",
               PLUGIN_HELPER_EXECUTABLE,
               std::strerror(errno));
        return;
    }
    
    if (!WIFEXITED(status)) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN, "%s terminated abnormally", PLUGIN_HELPER_EXECUTABLE);
        return;
    }
    
    int exitStatus = WEXITSTATUS(status);
    if (exitStatus != 0) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
                 "%s returned non-zero exit status (%d)",
                 PLUGIN_HELPER_EXECUTABLE,
                 exitStatus);
    }
}


bool NIDAQDevice::sendHelperRequest() {
    controlChannel.postRequest();
    if (!(controlChannel.waitForResponse(boost::posix_time::seconds(10)))) {
        merror(M_IODEVICE_MESSAGE_DOMAIN, "Internal error: Request to %s timed out", PLUGIN_HELPER_EXECUTABLE);
        return false;
    }
    
    HelperControlMessage &m = *controlMessage;
    HelperControlMessage::signed_int responseCode = m.code;
    
    switch (responseCode) {
        case HelperControlMessage::RESPONSE_OK:
            // Success
            break;
            
        case HelperControlMessage::RESPONSE_NIDAQ_ERROR:
            merror(M_IODEVICE_MESSAGE_DOMAIN, "%s", m.nidaqError.what.c_str());
            break;
            
        case HelperControlMessage::RESPONSE_EXCEPTION:
            merror(M_IODEVICE_MESSAGE_DOMAIN,
                   "Internal error: Exception in %s: %s",
                   PLUGIN_HELPER_EXECUTABLE,
                   m.exception.what.c_str());
            break;
            
        case HelperControlMessage::RESPONSE_BAD_REQUEST:
            merror(M_IODEVICE_MESSAGE_DOMAIN,
                   "Internal error: Bad request to %s: %s",
                   PLUGIN_HELPER_EXECUTABLE,
                   m.badRequest.info.c_str());
            break;
            
        default:
            merror(M_IODEVICE_MESSAGE_DOMAIN,
                   "Internal error: Unknown response code (%d) from %s",
                   responseCode,
                   PLUGIN_HELPER_EXECUTABLE);
            break;
    }
    
    return (responseCode == HelperControlMessage::RESPONSE_OK);
}


END_NAMESPACE_MW




























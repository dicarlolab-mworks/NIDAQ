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
const std::string NIDAQDevice::ANALOG_INPUT_DATA_INTERVAL("analog_input_data_interval");
const std::string NIDAQDevice::ANALOG_INPUT_UPDATE_INTERVAL("analog_input_update_interval");


void NIDAQDevice::describeComponent(ComponentInfo &info) {
    IODevice::describeComponent(info);
    
    info.setSignature("iodevice/nidaq");
    
    info.addParameter(NAME, true, "Dev1");
    info.addParameter(ANALOG_INPUT_DATA_INTERVAL, true, "1ms");
    info.addParameter(ANALOG_INPUT_UPDATE_INTERVAL, true, "3ms");
}


static std::string generateUniqueID() {
    boost::uint64_t uniqueID;
    if (RAND_pseudo_bytes(reinterpret_cast<unsigned char *>(&uniqueID), sizeof(uniqueID)) < 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                              "Internal error: Unable to generate unique identifier for NIDAQ device shared resource");
    }
    return (boost::format("%x") % uniqueID).str();
}


NIDAQDevice::NIDAQDevice(const ParameterValueMap &parameters) :
    IODevice(parameters),
    deviceName(parameters[NAME].str()),
    analogInputDataInterval(parameters[ANALOG_INPUT_DATA_INTERVAL]),
    analogInputUpdateInterval(parameters[ANALOG_INPUT_UPDATE_INTERVAL]),
    requestSemName(generateUniqueID()),
    responseSemName(generateUniqueID()),
    controlChannel(boost::interprocess::create_only, requestSemName, responseSemName),
    sharedMemoryName(generateUniqueID()),
    sharedMemory(boost::interprocess::create_only, sharedMemoryName),
    controlMessage(NULL),
    helperPID(-1)
{
    if (analogInputDataInterval <= 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid analog input data interval");
    }
    if (analogInputUpdateInterval <= 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid analog input update interval");
    }
    if (analogInputUpdateInterval % analogInputDataInterval != 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                              "Analog input update interval must be an integer multiple of analog input data interval");
    }
}


NIDAQDevice::~NIDAQDevice() {
    reapHelper();
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
    
    throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid channel type for NIDAQ device");
}


bool NIDAQDevice::initialize() {
    if (analogInputChannels.empty()) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "NIDAQ device must have at least one channel");
    }
    analogInputSampleBufferSize = (size_t(analogInputUpdateInterval / analogInputDataInterval) *
                                   analogInputChannels.size());
    
    sharedMemory.setSize(HelperControlMessage::sizeWithSamples(analogInputSampleBufferSize));
    controlMessage = sharedMemory.getMessagePtr();
    
    spawnHelper();
    
    mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Configuring NIDAQ device \"%s\"...", deviceName.c_str());
    
    if (!createTasks()) {
        return false;
    }
    
    mprintf(M_IODEVICE_MESSAGE_DOMAIN, "NIDAQ device \"%s\" is ready", deviceName.c_str());
    
    return true;
}


bool NIDAQDevice::startDeviceIO() {
    scoped_lock lock(controlMutex);
    
    if (analogInputScheduleTask) {
        merror(M_IODEVICE_MESSAGE_DOMAIN, "NIDAQ device is already running");
        return false;
    }
    
    controlMessage->code = HelperControlMessage::REQUEST_START_ANALOG_INPUT_TASK;
    if (!sendHelperRequest()) {
        return false;
    }
    
    boost::shared_ptr<NIDAQDevice> sharedThis = component_shared_from_this<NIDAQDevice>();
    
    analogInputScheduleTask = Scheduler::instance()->scheduleUS(FILELINE,
                                                                0,
                                                                analogInputUpdateInterval,
                                                                M_REPEAT_INDEFINITELY,
                                                                boost::bind(&NIDAQDevice::readAnalogInput, sharedThis),
                                                                M_DEFAULT_IODEVICE_PRIORITY,
                                                                M_DEFAULT_IODEVICE_WARN_SLOP_US,
                                                                M_DEFAULT_IODEVICE_FAIL_SLOP_US,
                                                                M_MISSED_EXECUTION_DROP);
    
    return true;
}


bool NIDAQDevice::stopDeviceIO() {
    scoped_lock lock(controlMutex);
    
    if (!analogInputScheduleTask) {
        return true;
    }
    
    analogInputScheduleTask->cancel();
    analogInputScheduleTask.reset();
    
    controlMessage->code = HelperControlMessage::REQUEST_STOP_ANALOG_INPUT_TASK;
    if (!sendHelperRequest()) {
        return false;
    }
    
    return true;
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


bool NIDAQDevice::createTasks() {
    if (analogInputChannels.size() > 0) {
        // Create the analog input channels
        BOOST_FOREACH(boost::shared_ptr<NIDAQAnalogInputVoltageChannel> channel, analogInputChannels) {
            controlMessage->code = HelperControlMessage::REQUEST_CREATE_ANALOG_INPUT_VOLTAGE_CHANNEL;
            
            controlMessage->analogVoltageChannel.channelNumber = channel->getChannelNumber();
            controlMessage->analogVoltageChannel.minVal = channel->getRangeMin();
            controlMessage->analogVoltageChannel.maxVal = channel->getRangeMax();
            
            if (!sendHelperRequest()) {
                return false;
            }
        }
        
        // Set analog input task's sample clock timing
        controlMessage->code = HelperControlMessage::REQUEST_SET_ANALOG_INPUT_SAMPLE_CLOCK_TIMING;
        controlMessage->sampleClockTiming.samplingRate = 1.0 / (analogInputDataInterval / 1e6);
        controlMessage->sampleClockTiming.samplesPerChannelToAcquire = (analogInputUpdateInterval /
                                                                        analogInputDataInterval);
        if (!sendHelperRequest()) {
            return false;
        }
    }
    
    return true;
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
            merror(M_IODEVICE_MESSAGE_DOMAIN,
                   "NIDAQ error: %s (%d)",
                   m.nidaqError.message.c_str(),
                   m.nidaqError.code);
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


void* NIDAQDevice::readAnalogInput() {
    scoped_lock lock(controlMutex);
    
    if (!analogInputScheduleTask) {
        // If we've already been canceled, don't try to read more data
        return NULL;
    }
    
    controlMessage->code = HelperControlMessage::REQUEST_READ_ANALOG_INPUT_SAMPLES;
    controlMessage->analogSamples.timeout = double(analogInputUpdateInterval) / 1e6;
    controlMessage->analogSamples.samples.numSamples = analogInputSampleBufferSize;
    
    if (!sendHelperRequest()) {
        return NULL;
    }
    
    const size_t numSamplesRead = controlMessage->analogSamples.samples.numSamples;
    if (numSamplesRead != analogInputSampleBufferSize) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
                 "NIDAQ device requested %u analog samples but got only %u",
                 analogInputSampleBufferSize,
                 numSamplesRead);
    }
    
    const size_t numChannels = analogInputChannels.size();
    for (size_t i = 0; i < numSamplesRead; i++) {
        double sample = controlMessage->analogSamples.samples[i];
        analogInputChannels[i % numChannels]->getVariable()->setValue(sample);
    }
    
    return NULL;
}


END_NAMESPACE_MW




























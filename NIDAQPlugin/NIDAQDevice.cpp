//
//  NIDAQDevice.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQDevice.h"

#include <crt_externs.h>
#include <cstring>
#include <spawn.h>
#include <sys/wait.h>

#include <openssl/rand.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/scope_exit.hpp>


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
    std::uint64_t uniqueID;
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
    analogOutputTaskRunning(false),
    digitalInputSampleBufferSize(0),
    digitalInputTaskRunning(false),
    digitalOutputSampleBufferSize(0),
    digitalOutputTasksRunning(false),
    counterInputCountEdgesTasksRunning(false)
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
    
    boost::shared_ptr<NIDAQAnalogOutputVoltageChannel> aoChannel;
    aoChannel = boost::dynamic_pointer_cast<NIDAQAnalogOutputVoltageChannel>(child);
    if (aoChannel) {
        boost::shared_ptr<NIDAQDevice> sharedThis = component_shared_from_this<NIDAQDevice>();
        boost::shared_ptr<AnalogOutputVoltageNotification> notification(new AnalogOutputVoltageNotification(sharedThis));
        aoChannel->addNewVoltageNotification(notification);
        
        analogOutputVoltageChannels.push_back(aoChannel);
        return;
    }
    
    boost::shared_ptr<NIDAQAnalogOutputVoltageWaveformChannel> aoVWChannel;
    aoVWChannel = boost::dynamic_pointer_cast<NIDAQAnalogOutputVoltageWaveformChannel>(child);
    if (aoVWChannel) {
        MWTime period = aoVWChannel->getPeriod();
        if (period % analogOutputDataInterval != 0) {
            throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                                  "Analog output waveform period must be an integer multiple of analog output data interval");
        }
        
        // This is the per-channel buffer size.  We'll multiply by the number of channels in initialize().
        analogOutputSampleBufferSize = std::max(analogOutputSampleBufferSize,
                                                std::size_t(period / analogOutputDataInterval));
        
        analogOutputVoltageWaveformChannels.push_back(aoVWChannel);
        return;
    }
    
    boost::shared_ptr<NIDAQDigitalInputChannel> diChannel = boost::dynamic_pointer_cast<NIDAQDigitalInputChannel>(child);
    if (diChannel) {
        digitalInputChannels.push_back(diChannel);
        return;
    }
    
    boost::shared_ptr<NIDAQDigitalOutputChannel> doChannel = boost::dynamic_pointer_cast<NIDAQDigitalOutputChannel>(child);
    if (doChannel) {
        boost::shared_ptr<NIDAQDevice> sharedThis = component_shared_from_this<NIDAQDevice>();
        const int portNumber = doChannel->getPortNumber();
        boost::shared_ptr<DigitalOutputLineStateNotification> notification(new DigitalOutputLineStateNotification(sharedThis, portNumber));
        
        doChannel->addNewWordNotification(notification);
        
        for (std::size_t lineNumber = 0; lineNumber < doChannel->getNumLinesInPort(); lineNumber++) {
            doChannel->addNewLineStateNotification(lineNumber, notification);
        }
        
        digitalOutputChannels[portNumber] = doChannel;
        return;
    }
    
    boost::shared_ptr<NIDAQCounterInputCountEdgesChannel> ciCEChannel = boost::dynamic_pointer_cast<NIDAQCounterInputCountEdgesChannel>(child);
    if (ciCEChannel) {
        counterInputCountEdgesChannels[ciCEChannel->getCounterNumber()] = ciCEChannel;
        return;
    }
    
    throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid channel type for NIDAQ device");
}


bool NIDAQDevice::initialize() {
    if (!(haveAnalogInputChannels() ||
          haveAnalogOutputChannels() ||
          haveDigitalInputChannels() ||
          haveDigitalOutputChannels() ||
          haveCounterInputCountEdgesChannels()))
    {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "NIDAQ device must have at least one channel");
    }
    
    if (haveAnalogOutputVoltageChannels() && haveAnalogOutputVoltageWaveformChannels()) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                              "NIDAQ device cannot use static and waveform analog output voltage channels simultaneously");
    }
    
    analogInputSampleBufferSize = (std::size_t(updateInterval / analogInputDataInterval) *
                                   analogInputChannels.size());
    if (haveAnalogOutputVoltageChannels()) {
        analogOutputSampleBufferSize = analogOutputVoltageChannels.size();
    } else {
        analogOutputSampleBufferSize *= analogOutputVoltageWaveformChannels.size();
    }
    digitalInputSampleBufferSize = digitalInputChannels.size();
    digitalOutputSampleBufferSize = (digitalOutputChannels.empty() ? 0 : 1);
    
    sharedMemory.setSize(HelperControlMessage::sizeWithSamples(std::max(analogInputSampleBufferSize,
                                                                        analogOutputSampleBufferSize),
                                                               std::max(digitalInputSampleBufferSize,
                                                                        digitalOutputSampleBufferSize)));
    controlMessage = sharedMemory.getMessagePtr();
    
    spawnHelper();
    
    mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Configuring NIDAQ device \"%s\"...", deviceName.c_str());
    
    if (!createTasks()) {
        return false;
    }
    
    mprintf(M_IODEVICE_MESSAGE_DOMAIN, "NIDAQ device \"%s\" is ready", deviceName.c_str());
    
    return true;
}


bool NIDAQDevice::createTasks() {
    if (haveAnalogInputChannels() && !createAnalogInputTask()) {
        return false;
    }
    if (haveAnalogOutputChannels() && !createAnalogOutputTask()) {
        return false;
    }
    if (haveDigitalInputChannels() && !createDigitalInputTask()) {
        return false;
    }
    if (haveDigitalOutputChannels() && !createDigitalOutputTasks()) {
        return false;
    }
    if (haveCounterInputCountEdgesChannels() && !createCounterInputCountEdgesTasks()) {
        return false;
    }
    
    return true;
}


bool NIDAQDevice::createAnalogInputTask() {
    BOOST_FOREACH(const boost::shared_ptr<NIDAQAnalogInputVoltageChannel> &channel, analogInputChannels) {
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
    if (haveAnalogOutputVoltageChannels()) {
        
        for (auto &channel : analogOutputVoltageChannels) {
            controlMessage->code = HelperControlMessage::REQUEST_CREATE_ANALOG_OUTPUT_VOLTAGE_CHANNEL;
            
            controlMessage->analogVoltageChannel.channelNumber = channel->getChannelNumber();
            controlMessage->analogVoltageChannel.minVal = channel->getRangeMin();
            controlMessage->analogVoltageChannel.maxVal = channel->getRangeMax();
            
            if (!sendHelperRequest()) {
                return false;
            }
        }
        
    } else {
        
        for (auto &channel : analogOutputVoltageWaveformChannels) {
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
                                                                        analogOutputVoltageWaveformChannels.size());
        if (!sendHelperRequest()) {
            return false;
        }
        
    }
    
    return true;
}


bool NIDAQDevice::createDigitalInputTask() {
    BOOST_FOREACH(const boost::shared_ptr<NIDAQDigitalInputChannel> &channel, digitalInputChannels) {
        controlMessage->code = HelperControlMessage::REQUEST_CREATE_DIGITAL_INPUT_CHANNEL;
        controlMessage->digitalChannel.portNumber = channel->getPortNumber();
        
        if (!sendHelperRequest()) {
            return false;
        }
    }
    
    return true;
}


bool NIDAQDevice::createDigitalOutputTasks() {
    BOOST_FOREACH(const DigitalOutputChannelMap::value_type &value, digitalOutputChannels) {
        controlMessage->code = HelperControlMessage::REQUEST_CREATE_DIGITAL_OUTPUT_CHANNEL;
        controlMessage->digitalChannel.portNumber = value.first;
        
        if (!sendHelperRequest()) {
            return false;
        }
    }
    
    return true;
}


bool NIDAQDevice::createCounterInputCountEdgesTasks() {
    BOOST_FOREACH(const CounterInputCountEdgesChannelMap::value_type &value, counterInputCountEdgesChannels) {
        controlMessage->code = HelperControlMessage::REQUEST_CREATE_COUNTER_INPUT_COUNT_EDGES_CHANNEL;
        controlMessage->counterChannel.counterNumber = value.first;
        
        if (!sendHelperRequest()) {
            return false;
        }
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
    if (haveDigitalOutputChannels() && !startDigitalOutputTasks()) {
        return false;
    }
    if (haveDigitalInputChannels() && !startDigitalInputTask()) {
        return false;
    }
    if (haveCounterInputCountEdgesChannels() && !startCounterInputCountEdgesTasks()) {
        return false;
    }
    
    if ((haveAnalogInputChannels() || haveDigitalInputChannels() || haveCounterInputCountEdgesChannels()) &&
        !readInputScheduleTask)
    {
        boost::shared_ptr<NIDAQDevice> sharedThis = component_shared_from_this<NIDAQDevice>();
        readInputScheduleTask = Scheduler::instance()->scheduleUS(FILELINE,
                                                                  updateInterval,
                                                                  updateInterval,
                                                                  M_REPEAT_INDEFINITELY,
                                                                  boost::bind(&NIDAQDevice::readInput, sharedThis),
                                                                  M_DEFAULT_IODEVICE_PRIORITY,
                                                                  M_DEFAULT_IODEVICE_WARN_SLOP_US,
                                                                  M_DEFAULT_IODEVICE_FAIL_SLOP_US,
                                                                  M_MISSED_EXECUTION_DROP);
    }
    
    return true;
}


bool NIDAQDevice::startAnalogOutputTask() {
    if (!analogOutputTaskRunning) {
        // Waveform samples must be written *before* starting the task
        if (haveAnalogOutputVoltageWaveformChannels()) {
            if (!writeAnalogOutput()) {
                return false;
            }
        }
        
        controlMessage->code = HelperControlMessage::REQUEST_START_ANALOG_OUTPUT_TASK;
        if (!sendHelperRequest()) {
            return false;
        }
        
        // Static samples must be written *after* starting the task
        if (haveAnalogOutputVoltageChannels()) {
            if (!writeAnalogOutput()) {
                return false;
            }
        }
        
        analogOutputTaskRunning = true;
    }
    
    return true;
}


bool NIDAQDevice::startAnalogInputTask() {
    if (!analogInputTaskRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_START_ANALOG_INPUT_TASK;
        if (!sendHelperRequest()) {
            return false;
        }
        
        MachClock::time_type systemBaseTimeNS = Clock::instance()->getSystemBaseTimeNS();
        analogInputStartTime = MWTime(controlMessage->taskStartTime - systemBaseTimeNS) / MWTime(1000);  // ns to us
        totalNumAnalogInputSamplesAcquired = 0;
        
        analogInputTaskRunning = true;
    }
    
    return true;
}


bool NIDAQDevice::startDigitalOutputTasks() {
    if (!digitalOutputTasksRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_START_DIGITAL_OUTPUT_TASKS;
        if (!sendHelperRequest()) {
            return false;
        }
        
        // Write the initial output values *after* starting the task, as writing before starting
        // fails with some devices (e.g. the NI USB-6501)
        BOOST_FOREACH(const DigitalOutputChannelMap::value_type &value, digitalOutputChannels) {
            if (!writeDigitalOutput(value.first)) {
                return false;
            }
        }
        
        digitalOutputTasksRunning = true;
    }
    
    return true;
}


bool NIDAQDevice::startDigitalInputTask() {
    if (!digitalInputTaskRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_START_DIGITAL_INPUT_TASK;
        if (!sendHelperRequest()) {
            return false;
        }
        
        digitalInputTaskRunning = true;
    }
    
    return true;
}


bool NIDAQDevice::startCounterInputCountEdgesTasks() {
    if (!counterInputCountEdgesTasksRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_START_COUNTER_INPUT_COUNT_EDGES_TASKS;
        if (!sendHelperRequest()) {
            return false;
        }
        
        counterInputCountEdgesTasksRunning = true;
    }
    
    return true;
}


bool NIDAQDevice::stopDeviceIO() {
    scoped_lock lock(controlMutex);
    
    if (readInputScheduleTask) {
        readInputScheduleTask->cancel();
        readInputScheduleTask.reset();
    }
    
    if (!(counterInputCountEdgesTasksRunning ||
          digitalInputTaskRunning ||
          digitalOutputTasksRunning ||
          analogInputTaskRunning ||
          analogOutputTaskRunning))
    {
        return true;
    }
    
    bool success = true;
    
    if (counterInputCountEdgesTasksRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_CLEAR_COUNTER_INPUT_COUNT_EDGES_TASKS;
        if (!sendHelperRequest()) {
            success = false;
        } else {
            counterInputCountEdgesTasksRunning = false;
        }
    }
    
    if (digitalInputTaskRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_CLEAR_DIGITAL_INPUT_TASK;
        if (!sendHelperRequest()) {
            success = false;
        } else {
            digitalInputTaskRunning = false;
        }
    }
    
    if (digitalOutputTasksRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_CLEAR_DIGITAL_OUTPUT_TASKS;
        if (!sendHelperRequest()) {
            success = false;
        } else {
            digitalOutputTasksRunning = false;
        }
    }
    
    if (analogInputTaskRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_CLEAR_ANALOG_INPUT_TASK;
        if (!sendHelperRequest()) {
            success = false;
        } else {
            analogInputTaskRunning = false;
        }
    }
    
    if (analogOutputTaskRunning) {
        controlMessage->code = HelperControlMessage::REQUEST_CLEAR_ANALOG_OUTPUT_TASK;
        if (!sendHelperRequest()) {
            success = false;
        } else {
            analogOutputTaskRunning = false;
        }
    }
    
    if (!success) {
        return false;
    }
    
    // Recreate the tasks so they're ready to go if startDeviceIO is called again
    if (!createTasks()) {
        return false;
    }
    
    return true;
}


void* NIDAQDevice::readInput() {
    scoped_lock lock(controlMutex);
    
    if (!readInputScheduleTask) {
        // If we've already been canceled, don't try to read more data
        return NULL;
    }
    
    if (haveDigitalInputChannels()) {
        readDigitalInput();
    }
    
    if (haveCounterInputCountEdgesChannels()) {
        readEdgeCounts();
    }
    
    // Read analog input last, since the read may block until the requested number of samples are available
    if (haveAnalogInputChannels()) {
        readAnalogInput();
    }
    
    return NULL;
}


void NIDAQDevice::readAnalogInput() {
    controlMessage->code = HelperControlMessage::REQUEST_READ_ANALOG_INPUT_SAMPLES;
    controlMessage->analogSamples.timeout = double(updateInterval) / 1e6;  // us to s
    controlMessage->analogSamples.samples.numSamples = analogInputSampleBufferSize;
    
    if (!sendHelperRequest()) {
        return;
    }
    
    const std::size_t numSamplesRead = controlMessage->analogSamples.samples.numSamples;
    const std::size_t numChannels = analogInputChannels.size();
    const MWTime firstSampleTime = analogInputStartTime + (totalNumAnalogInputSamplesAcquired / numChannels *
                                                           analogInputDataInterval);
    
    // Do this here in case there's an exception while posting the samples
    totalNumAnalogInputSamplesAcquired += numSamplesRead;
    
    if (numSamplesRead != analogInputSampleBufferSize) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
                 "NIDAQ device requested %lu analog samples but got only %lu",
                 analogInputSampleBufferSize,
                 numSamplesRead);
    }
    
    for (std::size_t i = 0; i < numSamplesRead; i++) {
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
}


bool NIDAQDevice::writeAnalogOutput() {
    std::size_t numChannels;
    if (haveAnalogOutputVoltageChannels()) {
        numChannels = analogOutputVoltageChannels.size();
    } else {
        numChannels = analogOutputVoltageWaveformChannels.size();
    }
    
    controlMessage->code = HelperControlMessage::REQUEST_WRITE_ANALOG_OUTPUT_SAMPLES;
    controlMessage->analogSamples.timeout = 10.0;
    controlMessage->analogSamples.samples.numSamples = analogOutputSampleBufferSize;
    
    for (std::size_t i = 0; i < analogOutputSampleBufferSize; i++) {
        const std::size_t channelNum = i % numChannels;
        double &sample = controlMessage->analogSamples.samples[i];
        
        if (haveAnalogOutputVoltageChannels()) {
            sample = analogOutputVoltageChannels[channelNum]->getVoltage();
        } else {
            MWTime sampleTime = analogOutputDataInterval * (i / numChannels);
            sample = analogOutputVoltageWaveformChannels[channelNum]->getSampleForTime(sampleTime);
        }
    }
    
    if (!sendHelperRequest()) {
        return false;
    }
    
    const std::size_t numSamplesWritten = controlMessage->analogSamples.samples.numSamples;
    
    if (numSamplesWritten != analogOutputSampleBufferSize) {
        merror(M_IODEVICE_MESSAGE_DOMAIN,
               "Wrote only %lu of %lu analog output samples to NIDAQ device",
               numSamplesWritten,
               analogOutputSampleBufferSize);
        return false;
    }
    
    return true;
}


void NIDAQDevice::readDigitalInput() {
    controlMessage->code = HelperControlMessage::REQUEST_READ_DIGITAL_INPUT_SAMPLES;
    controlMessage->digitalSamples.timeout = double(updateInterval) / 1e6;  // us to s
    controlMessage->digitalSamples.samples.numSamples = digitalInputSampleBufferSize;
    
    if (!sendHelperRequest()) {
        return;
    }
    
    const std::size_t numSamplesRead = controlMessage->digitalSamples.samples.numSamples;
    const std::size_t numChannels = digitalInputChannels.size();
    const MWTime sampleTime = Clock::instance()->getCurrentTimeUS();
    
    if (numSamplesRead != digitalInputSampleBufferSize) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
                 "NIDAQ device requested %lu digital samples but got only %lu",
                 digitalInputSampleBufferSize,
                 numSamplesRead);
    }
    
    std::size_t channelNumber = 0;
    std::size_t channelLineNumber = 0;
    
    for (std::size_t sampleNumber = 0; sampleNumber < numSamplesRead; sampleNumber++) {
        for (std::size_t sampleBitNumber = 0; sampleBitNumber < NIDAQDigitalInputChannel::maxNumLines; sampleBitNumber++) {
            bool lineState = bool(controlMessage->digitalSamples.samples[sampleNumber] & (1u << sampleBitNumber));
            
            digitalInputChannels[channelNumber]->postLineState(channelLineNumber, lineState, sampleTime);
            channelLineNumber++;
            
            if (channelLineNumber >= digitalInputChannels[channelNumber]->getNumLinesInPort()) {
                channelNumber++;
                if (channelNumber >= numChannels) {
                    return;
                }
                channelLineNumber = 0;
            }
        }
    }
}


bool NIDAQDevice::writeDigitalOutput(int portNumber) {
    controlMessage->code = HelperControlMessage::REQUEST_WRITE_DIGITAL_OUTPUT_SAMPLES;
    controlMessage->digitalSamples.portNumber = portNumber;
    controlMessage->digitalSamples.timeout = double(updateInterval) / 1e6;  // us to s
    controlMessage->digitalSamples.samples.numSamples = digitalOutputSampleBufferSize;
    
    const boost::shared_ptr<NIDAQDigitalOutputChannel> &channel = digitalOutputChannels[portNumber];
    
    for (std::size_t sampleNumber = 0; sampleNumber < digitalOutputSampleBufferSize; sampleNumber++) {
        std::uint32_t &sample = controlMessage->digitalSamples.samples[sampleNumber];
        sample = 0;
        
        for (std::size_t lineNumber = 0; lineNumber < channel->getNumLinesInPort(); lineNumber++) {
            bool lineState = channel->getLineState(lineNumber);
            sample |= (std::uint32_t(lineState) << lineNumber);
        }
    }
    
    if (!sendHelperRequest()) {
        return false;
    }
    
    const std::size_t numSamplesWritten = controlMessage->digitalSamples.samples.numSamples;
    
    if (numSamplesWritten != digitalOutputSampleBufferSize) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
                 "Wrote only %lu of %lu digital output samples to NIDAQ device",
                 numSamplesWritten,
                 digitalOutputSampleBufferSize);
        return false;
    }
    
    return true;
}


void NIDAQDevice::readEdgeCounts() {
    BOOST_FOREACH(const CounterInputCountEdgesChannelMap::value_type &value, counterInputCountEdgesChannels) {
        controlMessage->code = HelperControlMessage::REQUEST_READ_COUNTER_INPUT_COUNT_EDGES_VALUE;
        controlMessage->edgeCount.counterNumber = value.first;
        controlMessage->edgeCount.timeout = double(updateInterval) / 1e6;  // us to s
        
        if (sendHelperRequest()) {
            value.second->postCount(controlMessage->edgeCount.value, Clock::instance()->getCurrentTimeUS());
        }
    }
}


NIDAQDevice::~NIDAQDevice() {
    reapHelper();
}


void NIDAQDevice::spawnHelper() {
    posix_spawnattr_t attr;
    int status = posix_spawnattr_init(&attr);
    if (status != 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "posix_spawnattr_init failed", std::strerror(status));
    }
    BOOST_SCOPE_EXIT(&attr) {
        int status = posix_spawnattr_destroy(&attr);
        if (status != 0) {
            merror(M_IODEVICE_MESSAGE_DOMAIN, "posix_spawnattr_destroy failed: %s", std::strerror(status));
        }
    } BOOST_SCOPE_EXIT_END
    
    {
#ifdef __i386__
        cpu_type_t pref = CPU_TYPE_I386;
#else
        cpu_type_t pref = CPU_TYPE_X86_64;
#endif
        size_t ocount;
        status = posix_spawnattr_setbinpref_np(&attr, 1, &pref, &ocount);
        if (status != 0) {
            throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                                  "posix_spawnattr_setbinpref_np failed",
                                  std::strerror(status));
        }
    }
    
    const char * const argv[] = {
        PLUGIN_HELPER_EXECUTABLE,
        requestSemName.c_str(),
        responseSemName.c_str(),
        sharedMemoryName.c_str(),
        deviceName.c_str(),
        0
    };
    
    status = posix_spawn(&helperPID,
                         PLUGIN_HELPERS_DIR "/" PLUGIN_HELPER_EXECUTABLE,
                         NULL,
                         &attr,
                         const_cast<char * const *>(argv),
                         *_NSGetEnviron());
    
    if (status != 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                              "Unable to launch " PLUGIN_HELPER_EXECUTABLE,
                              std::strerror(status));
    }
    
    // Give helper process some time to start up
    Clock::instance()->sleepMS(1000);
    
    //
    // Verify that the helper has not exited
    //
    
    pid_t pid = waitpid(helperPID, &status, WNOHANG | WUNTRACED);
    
    if (-1 == pid) {
        merror(M_IODEVICE_MESSAGE_DOMAIN,
               "Error while checking status of %s: %s",
               PLUGIN_HELPER_EXECUTABLE,
               std::strerror(errno));
    } else if (pid != 0) {
        helperPID = -1;
        std::string msg(PLUGIN_HELPER_EXECUTABLE " ");
        
        if (WSTOPSIG(status)) {
            msg += "was stopped by a signal";
        } else if (WIFSIGNALED(status)) {
            msg += "terminated due to a signal";
        } else {
            msg += "exited prematurely";
        }
        
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, msg);
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
    int responseCode = m.code;
    
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


void NIDAQDevice::AnalogOutputVoltageNotification::notify(const Datum &data, MWTime time) {
    boost::shared_ptr<NIDAQDevice> nidaqDevice = nidaqDeviceWeak.lock();
    if (nidaqDevice) {
        scoped_lock lock(nidaqDevice->controlMutex);
        if (nidaqDevice->analogOutputTaskRunning) {
            nidaqDevice->writeAnalogOutput();
        }
    }
}


void NIDAQDevice::DigitalOutputLineStateNotification::notify(const Datum &data, MWTime time) {
    boost::shared_ptr<NIDAQDevice> nidaqDevice = nidaqDeviceWeak.lock();
    if (nidaqDevice) {
        scoped_lock lock(nidaqDevice->controlMutex);
        if (nidaqDevice->digitalOutputTasksRunning) {
            nidaqDevice->writeDigitalOutput(portNumber);
        }
    }
}


END_NAMESPACE_MW




























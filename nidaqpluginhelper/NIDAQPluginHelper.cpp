//
//  NIDAQPluginHelper.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQPluginHelper.h"

#include <boost/foreach.hpp>

#include "Error.h"


NIDAQPluginHelper::NIDAQPluginHelper(IPCRequestResponse &ipc,
                                     HelperControlMessage &message,
                                     const std::string &deviceName) :
    ipc(ipc),
    m(message),
    deviceName(deviceName)
{ }


bool NIDAQPluginHelper::handleRequests() {
    const boost::posix_time::time_duration timeout = boost::posix_time::minutes(60);
    bool done = false;
    
    while (!done) {
        if (!(ipc.waitForRequest(timeout))) {
            return false;
        }
        
        try {
            
            if (!device) {
                device.reset(new nidaq::Device(deviceName));
            }
            
            handleRequest(done);
            
        } catch (const NIDAQPluginHelperError &e) {
            
            m.code = HelperControlMessage::RESPONSE_BAD_REQUEST;
            m.badRequest.info = e.what();
            
        } catch (const nidaq::Error &e) {
            
            m.code = HelperControlMessage::RESPONSE_NIDAQ_ERROR;
            m.nidaqError.code = e.getCode();
            m.nidaqError.what = e.what();
            
        } catch (const std::exception &e) {
            
            m.code = HelperControlMessage::RESPONSE_EXCEPTION;
            m.exception.what = e.what();
            
        } catch (...) {
            
            m.code = HelperControlMessage::RESPONSE_EXCEPTION;
            m.exception.what = "Unknown exception";
            
        }
        
        ipc.postResponse();
    }
    
    return true;
}


void NIDAQPluginHelper::handleRequest(bool &done) {
    switch (m.code) {
        case HelperControlMessage::REQUEST_CREATE_ANALOG_INPUT_VOLTAGE_CHANNEL:
            createAnalogInputVoltageChannel();
            break;
            
        case HelperControlMessage::REQUEST_SET_ANALOG_INPUT_SAMPLE_CLOCK_TIMING:
            setAnalogInputSampleClockTiming();
            break;
            
        case HelperControlMessage::REQUEST_START_ANALOG_INPUT_TASK:
            startAnalogInputTask();
            break;
            
        case HelperControlMessage::REQUEST_READ_ANALOG_INPUT_SAMPLES:
            readAnalogInputSamples();
            break;
            
        case HelperControlMessage::REQUEST_CLEAR_ANALOG_INPUT_TASK:
            clearAnalogInputTask();
            break;
            
        case HelperControlMessage::REQUEST_CREATE_ANALOG_OUTPUT_VOLTAGE_CHANNEL:
            createAnalogOutputVoltageChannel();
            break;
            
        case HelperControlMessage::REQUEST_SET_ANALOG_OUTPUT_SAMPLE_CLOCK_TIMING:
            setAnalogOutputSampleClockTiming();
            break;
            
        case HelperControlMessage::REQUEST_START_ANALOG_OUTPUT_TASK:
            startAnalogOutputTask();
            break;
            
        case HelperControlMessage::REQUEST_WRITE_ANALOG_OUTPUT_SAMPLES:
            writeAnalogOutputSamples();
            break;
            
        case HelperControlMessage::REQUEST_CLEAR_ANALOG_OUTPUT_TASK:
            clearAnalogOutputTask();
            break;
            
        case HelperControlMessage::REQUEST_CREATE_DIGITAL_INPUT_CHANNEL:
            createDigitalInputChannel();
            break;
            
        case HelperControlMessage::REQUEST_START_DIGITAL_INPUT_TASK:
            startDigitalInputTask();
            break;
            
        case HelperControlMessage::REQUEST_READ_DIGITAL_INPUT_SAMPLES:
            readDigitalInputSamples();
            break;
            
        case HelperControlMessage::REQUEST_CLEAR_DIGITAL_INPUT_TASK:
            clearDigitalInputTask();
            break;
            
        case HelperControlMessage::REQUEST_CREATE_DIGITAL_OUTPUT_CHANNEL:
            createDigitalOutputChannel();
            break;
            
        case HelperControlMessage::REQUEST_START_DIGITAL_OUTPUT_TASKS:
            startDigitalOutputTasks();
            break;
            
        case HelperControlMessage::REQUEST_WRITE_DIGITAL_OUTPUT_SAMPLES:
            writeDigitalOutputSamples();
            break;
            
        case HelperControlMessage::REQUEST_CLEAR_DIGITAL_OUTPUT_TASKS:
            clearDigitalOutputTasks();
            break;
            
        case HelperControlMessage::REQUEST_SHUTDOWN:
            done = true;
            break;
            
        case HelperControlMessage::REQUEST_PING:
            // Nothing to do
            break;
            
        default:
            throw NIDAQPluginHelperError(boost::format("Unknown request code: %d") % m.code);
    }
    
    m.code = HelperControlMessage::RESPONSE_OK;
}


void NIDAQPluginHelper::createAnalogInputVoltageChannel() {
    device->getAnalogInputTask().addVoltageChannel(m.analogVoltageChannel.channelNumber,
                                                   m.analogVoltageChannel.minVal,
                                                   m.analogVoltageChannel.maxVal);
}


void NIDAQPluginHelper::setAnalogInputSampleClockTiming() {
    device->getAnalogInputTask().setSampleClockTiming(m.sampleClockTiming.samplingRate,
                                                      m.sampleClockTiming.samplesPerChannelToAcquire);
}


void NIDAQPluginHelper::startAnalogInputTask() {
    device->getAnalogInputTask().start();
    m.taskStartTime = clock.nanoTime();
}


void NIDAQPluginHelper::readAnalogInputSamples() {
    if (!(device->getAnalogInputTask().isRunning())) {
        throw NIDAQPluginHelperError("Analog input task is not running");
    }
    
    std::size_t numSamplesRead = device->getAnalogInputTask().read(m.analogSamples.samples,
                                                                   m.analogSamples.timeout,
                                                                   true);  // Group samples by scan number
    m.analogSamples.samples.numSamples = numSamplesRead;
}


void NIDAQPluginHelper::clearAnalogInputTask() {
    device->clearAnalogInputTask();
}


void NIDAQPluginHelper::createAnalogOutputVoltageChannel() {
    device->getAnalogOutputTask().addVoltageChannel(m.analogVoltageChannel.channelNumber,
                                                    m.analogVoltageChannel.minVal,
                                                    m.analogVoltageChannel.maxVal);
}


void NIDAQPluginHelper::setAnalogOutputSampleClockTiming() {
    device->getAnalogOutputTask().setSampleClockTiming(m.sampleClockTiming.samplingRate,
                                                       m.sampleClockTiming.samplesPerChannelToAcquire);
}


void NIDAQPluginHelper::startAnalogOutputTask() {
    device->getAnalogOutputTask().start();
}


void NIDAQPluginHelper::writeAnalogOutputSamples() {
    std::size_t numSamplesWritten = device->getAnalogOutputTask().write(m.analogSamples.samples,
                                                                        m.analogSamples.timeout,
                                                                        true);  // Group samples by scan number
    
    m.analogSamples.samples.numSamples = numSamplesWritten;
}


void NIDAQPluginHelper::clearAnalogOutputTask() {
    device->clearAnalogOutputTask();
}


void NIDAQPluginHelper::createDigitalInputChannel() {
    device->getDigitalInputTask().addChannel(m.digitalChannel.portNumber);
}


void NIDAQPluginHelper::startDigitalInputTask() {
    device->getDigitalInputTask().start();
}


void NIDAQPluginHelper::readDigitalInputSamples() {
    if (!(device->getDigitalInputTask().isRunning())) {
        throw NIDAQPluginHelperError("Digital input task is not running");
    }
    
    std::size_t numSamplesRead = device->getDigitalInputTask().read(m.digitalSamples.samples,
                                                                    m.digitalSamples.timeout,
                                                                    true);  // Group samples by scan number
    m.digitalSamples.samples.numSamples = numSamplesRead;
}


void NIDAQPluginHelper::clearDigitalInputTask() {
    device->clearDigitalInputTask();
}


void NIDAQPluginHelper::createDigitalOutputChannel() {
    device->getDigitalOutputTask(m.digitalChannel.portNumber);
    digitalOutputPortNumbers.insert(m.digitalChannel.portNumber);
}


void NIDAQPluginHelper::startDigitalOutputTasks() {
    BOOST_FOREACH(unsigned int portNumber, digitalOutputPortNumbers) {
        device->getDigitalOutputTask(portNumber).start();
    }
}


void NIDAQPluginHelper::writeDigitalOutputSamples() {
    nidaq::DigitalOutputTask &task = device->getDigitalOutputTask(m.digitalSamples.portNumber);
    
    std::size_t numSamplesWritten = task.write(m.digitalSamples.samples,
                                               m.digitalSamples.timeout);

    m.digitalSamples.samples.numSamples = numSamplesWritten;
}


void NIDAQPluginHelper::clearDigitalOutputTasks() {
    BOOST_FOREACH(unsigned int portNumber, digitalOutputPortNumbers) {
        device->clearDigitalOutputTask(portNumber);
        digitalOutputPortNumbers.erase(portNumber);
    }
}



























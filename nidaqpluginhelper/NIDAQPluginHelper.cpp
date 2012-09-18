//
//  NIDAQPluginHelper.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQPluginHelper.h"

#include "Error.h"


NIDAQPluginHelper::NIDAQPluginHelper(IPCRequestResponse &ipc,
                                     HelperControlMessage &message,
                                     const std::string &deviceName) :
    ipc(ipc),
    m(message),
    device(deviceName)
{ }


bool NIDAQPluginHelper::handleRequests() {
    const boost::posix_time::time_duration timeout = boost::posix_time::minutes(60);
    bool done = false;
    
    while (!done) {
        if (!(ipc.waitForRequest(timeout))) {
            return false;
        }
        
        try {
            
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
            
        case HelperControlMessage::REQUEST_STOP_ANALOG_INPUT_TASK:
            stopAnalogInputTask();
            break;
            
        case HelperControlMessage::REQUEST_READ_ANALOG_INPUT_SAMPLES:
            readAnalogInputSamples();
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
            
        case HelperControlMessage::REQUEST_STOP_ANALOG_OUTPUT_TASK:
            stopAnalogOutputTask();
            break;
            
        case HelperControlMessage::REQUEST_WRITE_ANALOG_OUTPUT_SAMPLES:
            writeAnalogOutputSamples();
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


void NIDAQPluginHelper::requireAnalogInputTask() {
    if (!analogInputTask) {
        analogInputTask.reset(new nidaq::AnalogInputTask(device));
    }
}


void NIDAQPluginHelper::createAnalogInputVoltageChannel() {
    requireAnalogInputTask();
    analogInputTask->addVoltageChannel(m.analogVoltageChannel.channelNumber,
                                       m.analogVoltageChannel.minVal,
                                       m.analogVoltageChannel.maxVal);
}


void NIDAQPluginHelper::setAnalogInputSampleClockTiming() {
    requireAnalogInputTask();
    analogInputTask->setSampleClockTiming(m.sampleClockTiming.samplingRate,
                                          m.sampleClockTiming.samplesPerChannelToAcquire);
}


void NIDAQPluginHelper::startAnalogInputTask() {
    requireAnalogInputTask();
    analogInputTask->start();
    m.taskStartTime = clock.nanoTime();
}


void NIDAQPluginHelper::stopAnalogInputTask() {
    requireAnalogInputTask();
    analogInputTask->stop();
}


void NIDAQPluginHelper::readAnalogInputSamples() {
    requireAnalogInputTask();
    if (!(analogInputTask->isRunning())) {
        throw NIDAQPluginHelperError("Analog input task is not running");
    }
    
    size_t numSamplesRead = analogInputTask->read(m.analogSamples.samples,
                                                  m.analogSamples.timeout,
                                                  true);  // Group samples by scan number
    m.analogSamples.samples.numSamples = numSamplesRead;
}


void NIDAQPluginHelper::requireAnalogOutputTask() {
    if (!analogOutputTask) {
        analogOutputTask.reset(new nidaq::AnalogOutputTask(device));
    }
}


void NIDAQPluginHelper::createAnalogOutputVoltageChannel() {
    requireAnalogOutputTask();
    analogOutputTask->addVoltageChannel(m.analogVoltageChannel.channelNumber,
                                        m.analogVoltageChannel.minVal,
                                        m.analogVoltageChannel.maxVal);
}


void NIDAQPluginHelper::setAnalogOutputSampleClockTiming() {
    requireAnalogOutputTask();
    analogOutputTask->setSampleClockTiming(m.sampleClockTiming.samplingRate,
                                           m.sampleClockTiming.samplesPerChannelToAcquire);
}


void NIDAQPluginHelper::startAnalogOutputTask() {
    requireAnalogOutputTask();
    analogOutputTask->start();
}


void NIDAQPluginHelper::stopAnalogOutputTask() {
    requireAnalogOutputTask();
    analogOutputTask->stop();
}


void NIDAQPluginHelper::writeAnalogOutputSamples() {
    requireAnalogOutputTask();
    
    size_t numSamplesWritten = analogOutputTask->write(m.analogSamples.samples,
                                                       m.analogSamples.timeout,
                                                       true);  // Group samples by scan number
    
    m.analogSamples.samples.numSamples = numSamplesWritten;
}



























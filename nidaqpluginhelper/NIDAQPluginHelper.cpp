//
//  NIDAQPluginHelper.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQPluginHelper.h"

#include "Error.h"

#include "NIDAQPluginHelperError.h"


NIDAQPluginHelper::NIDAQPluginHelper(const std::string &deviceName,
                                     const std::string &wantRequestName,
                                     const std::string &wantResponseName,
                                     HelperControlMessage &message) :
    device(deviceName),
    ipc(boost::interprocess::open_only, wantRequestName, wantResponseName),
    m(message)
{ }


bool NIDAQPluginHelper::handleControlRequests() {
    const boost::posix_time::time_duration timeout = boost::posix_time::minutes(60);
    bool done = false;
    
    while (!done) {
        if (!(ipc.waitForRequest(timeout))) {
            return false;
        }
        
        try {
            
            handleControlRequest(done);
            
        } catch (const NIDAQPluginHelperError &e) {
            
            m.code = HelperControlMessage::RESPONSE_BAD_REQUEST;
            m.badRequest.info = e.what();
            
        } catch (const nidaq::Error &e) {
            
            m.code = HelperControlMessage::RESPONSE_NIDAQ_ERROR;
            m.nidaqError.code = e.getCode();
            m.nidaqError.message = e.getMessage();
            
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


void NIDAQPluginHelper::handleControlRequest(bool &done) {
    const HelperControlMessage::signed_int requestCode = m.code;
    HelperControlMessage::signed_int responseCode = HelperControlMessage::RESPONSE_OK;
    
    switch (requestCode) {
        case HelperControlMessage::REQUEST_GET_DEVICE_SERIAL_NUMBER:
            m.deviceSerialNumber = device.getSerialNumber();
            break;
            
        case HelperControlMessage::REQUEST_CREATE_ANALOG_INPUT_TASK:
            createAnalogInputTask();
            break;
            
        case HelperControlMessage::REQUEST_CREATE_ANALOG_INPUT_VOLTAGE_CHANNEL:
            createAnalogInputVoltageChannel(m.createAnalogInputVoltageChannel.channelNumber,
                                            m.createAnalogInputVoltageChannel.minVal,
                                            m.createAnalogInputVoltageChannel.maxVal);
            break;
            
        case HelperControlMessage::REQUEST_SET_ANALOG_INPUT_SAMPLE_CLOCK_TIMING:
            setAnalogInputSampleClockTiming(m.setAnalogInputSampleClockTiming.samplingRate);
            break;
            
        case HelperControlMessage::REQUEST_START_ALL_TASKS:
            startAllTasks();
            break;
            
        case HelperControlMessage::REQUEST_STOP_ALL_TASKS:
            stopAllTasks();
            break;
            
        case HelperControlMessage::REQUEST_SHUTDOWN:
            done = true;
            break;
            
        default:
            throw NIDAQPluginHelperError(boost::format("Unknown request code: %d") % requestCode);
    }
    
    m.code = responseCode;
}


void NIDAQPluginHelper::createAnalogInputTask() {
    if (analogInputTask) {
        throw NIDAQPluginHelperError("Analog input task already created");
    }
    analogInputTask.reset(new nidaq::AnalogInputTask(device));
}


void NIDAQPluginHelper::createAnalogInputVoltageChannel(unsigned int channelNumber, double minVal, double maxVal) {
    if (!analogInputTask) {
        throw NIDAQPluginHelperError("Analog input task has not been created");
    }
    analogInputTask->addVoltageChannel(channelNumber, minVal, maxVal);
}


void NIDAQPluginHelper::setAnalogInputSampleClockTiming(double samplingRate) {
    if (!analogInputTask) {
        throw NIDAQPluginHelperError("Analog input task has not been created");
    }
    analogInputTask->setSampleClockTiming(samplingRate);
}


void NIDAQPluginHelper::startAllTasks() {
    if (analogInputTask) {
        analogInputTask->start();
    }
}


void NIDAQPluginHelper::stopAllTasks() {
    if (analogInputTask) {
        analogInputTask->stop();
    }
}




























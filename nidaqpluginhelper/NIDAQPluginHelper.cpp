//
//  NIDAQPluginHelper.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQPluginHelper.h"

#include "Error.h"


NIDAQPluginHelper::NIDAQPluginHelper(const std::string &deviceName, HelperControlChannel &controlChannel) :
    device(deviceName),
    controlChannel(controlChannel)
{ }


bool NIDAQPluginHelper::handleControlRequests() {
    const boost::posix_time::time_duration receiveTimeout = boost::posix_time::minutes(60);
    const boost::posix_time::time_duration sendTimeout = boost::posix_time::seconds(10);
    
    HelperControlMessage &m = controlChannel.getMessage();
    bool done = false;
    
    while (!done) {
        if (!(controlChannel.receiveRequest(receiveTimeout))) {
            return false;
        }
        
        try {
            
            handleControlRequest(m, done);
            
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
        
        if (!(controlChannel.sendResponse(sendTimeout))) {
            return false;
        }
    }
    
    return true;
}


void NIDAQPluginHelper::handleControlRequest(HelperControlMessage &m, bool &done) {
    const HelperControlMessage::signed_int requestCode = m.code;
    HelperControlMessage::signed_int responseCode = HelperControlMessage::RESPONSE_OK;
    
    switch (requestCode) {
        case HelperControlMessage::REQUEST_GET_DEVICE_SERIAL_NUMBER:
            m.deviceSerialNumber = device.getSerialNumber();
            break;
            
        case HelperControlMessage::REQUEST_SHUTDOWN:
            done = true;
            break;
            
        default:
            responseCode = HelperControlMessage::RESPONSE_BAD_REQUEST;
            m.badRequest.info = boost::format("Unknown request code: %d") % requestCode;
            break;
    }
    
    m.code = responseCode;
}





























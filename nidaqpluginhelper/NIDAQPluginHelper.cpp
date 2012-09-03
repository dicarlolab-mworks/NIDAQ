//
//  NIDAQPluginHelper.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQPluginHelper.h"

#include "Error.h"


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





























//
//  NIDAQError.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQError.h"

#include <iostream>
#include <vector>

#include <boost/format.hpp>


std::string NIDAQError::getExtendedErrorInfo() {
    uInt32 messageSize = DAQmxBaseGetExtendedErrorInfo(NULL, 0);
    std::vector<char> messageData(messageSize, 0);
    char *messageBuffer = &(messageData.front());
    
    std::string message("Unknown NIDAQmxBase error.");
    
    if (!failed(DAQmxBaseGetExtendedErrorInfo(messageBuffer, messageSize))) {
        message = messageBuffer;
    }
    
    return message;
}


std::string NIDAQError::formatErrorMessage(int32 code, const std::string &message) {
    return (boost::format("NIDAQmxBase error %d: %s") % code % message).str();
}


void NIDAQError::logErrorMessage(int32 code, const std::string &message) {
    std::cerr << formatErrorMessage(code, message) << std::endl;
}

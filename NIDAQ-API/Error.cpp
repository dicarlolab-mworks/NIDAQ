//
//  Error.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "Error.h"

#include <iostream>
#include <vector>

#include <boost/format.hpp>

#include "NIDAQmxBaseAPI.h"


BEGIN_NAMESPACE_NIDAQ


Error::Error(int32_t code, const std::string &message) :
    std::runtime_error(formatErrorMessage(code, message)),
    code(code),
    message(message)
{ }


std::string Error::getExtendedErrorInfo() {
    int32_t messageSize = DAQmxBaseGetExtendedErrorInfo(NULL, 0);
    std::vector<char> messageData(messageSize, 0);
    char *messageBuffer = &(messageData.front());
    
    std::string message("Unknown NIDAQmxBase error.");
    
    if (!failed(DAQmxBaseGetExtendedErrorInfo(messageBuffer, messageSize))) {
        message = messageBuffer;
    }
    
    return message;
}


std::string Error::formatErrorMessage(int32_t code, const std::string &message) {
    return (boost::format("NIDAQmxBase error %d: %s") % code % message).str();
}


void Error::logErrorMessage(int32_t code, const std::string &message) {
    std::cerr << formatErrorMessage(code, message) << std::endl;
}


END_NAMESPACE_NIDAQ




























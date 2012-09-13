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


Error::Error(const std::string &message, int32_t code) :
    std::runtime_error(formatErrorMessage(message, code)),
    message(message),
    code(code)
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


std::string Error::formatErrorMessage(const std::string &message, int32_t code) {
    std::string result("NIDAQ error: " + message);
    if (code != 0) {
        result += (boost::format(" (%d)") % code).str();
    }
    return result;
}


void Error::logErrorMessage(const std::string &message, int32_t code) {
    std::cerr << formatErrorMessage(message, code) << std::endl;
}


END_NAMESPACE_NIDAQ




























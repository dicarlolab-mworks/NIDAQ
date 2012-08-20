//
//  NIDAQError.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQError.h"

#include <vector>


std::string NIDAQError::getErrorMessage(int32 errorCode) {
    uInt32 messageSize = DAQmxBaseGetExtendedErrorInfo(NULL, 0);
    std::vector<char> messageData(messageSize, 0);
    char *messageBuffer = &(messageData.front());
    std::string message("Unknown NIDAQmxBase error");
    
    if (!DAQmxFailed(DAQmxBaseGetExtendedErrorInfo(messageBuffer, messageSize))) {
        message = messageBuffer;
    }
    
    return message;
}

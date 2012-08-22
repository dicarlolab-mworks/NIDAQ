//
//  NIDAQError.h
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __nidaqtest__NIDAQError__
#define __nidaqtest__NIDAQError__

#include <stdexcept>
#include <string>

#include "NIDAQTypes.h"


class NIDAQError : public std::runtime_error {
    
public:
    static bool failed(int32_t errorCode) {
        return (errorCode != 0);
    }
    
    static void throwIfFailed(int32_t errorCode) {
        if (failed(errorCode)) {
            throw NIDAQError(errorCode, getExtendedErrorInfo());
        }
    }
    
    static void logIfFailed(int32_t errorCode) {
        if (failed(errorCode)) {
            logErrorMessage(errorCode, getExtendedErrorInfo());
        }
    }
    
    NIDAQError(int32_t code, const std::string &message);
    
    int32_t getCode() const {
        return code;
    }
    
    const std::string& getMessage() const {
        return message;
    }
    
private:
    static std::string getExtendedErrorInfo();
    static std::string formatErrorMessage(int32_t code, const std::string &message);
    static void logErrorMessage(int32_t code, const std::string &message);
    
    const int32_t code;
    const std::string message;
    
};


#endif /* !defined(__nidaqtest__NIDAQError__) */




























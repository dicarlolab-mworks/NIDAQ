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

#include <NIDAQmxBase.h>


class NIDAQError : public std::runtime_error {
    
public:
    static bool failed(int32 errorCode) {
        return DAQmxFailed(errorCode);
    }
    
    static void throwOnFailure(int32 errorCode) {
        if (failed(errorCode)) {
            throw NIDAQError(errorCode, getExtendedErrorInfo());
        }
    }
    
    static void logOnFailure(int32 errorCode) {
        if (failed(errorCode)) {
            logErrorMessage(errorCode, getExtendedErrorInfo());
        }
    }
    
    NIDAQError(int32 code, const std::string &message);
    
    int32 getCode() const {
        return code;
    }
    
    const std::string& getMessage() const {
        return message;
    }
    
private:
    static std::string getExtendedErrorInfo();
    static std::string formatErrorMessage(int32 code, const std::string &message);
    static void logErrorMessage(int32 code, const std::string &message);
    
    const int32 code;
    const std::string message;
    
};


#endif /* !defined(__nidaqtest__NIDAQError__) */




























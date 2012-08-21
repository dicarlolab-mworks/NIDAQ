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


class NIDAQError : public std::runtime_error {
    
public:
    static bool failed(int errorCode) {
        return (errorCode != 0);
    }
    
    static void throwOnFailure(int errorCode) {
        if (failed(errorCode)) {
            throw NIDAQError(errorCode, getExtendedErrorInfo());
        }
    }
    
    static void logOnFailure(int errorCode) {
        if (failed(errorCode)) {
            logErrorMessage(errorCode, getExtendedErrorInfo());
        }
    }
    
    NIDAQError(int code, const std::string &message);
    
    int getCode() const {
        return code;
    }
    
    const std::string& getMessage() const {
        return message;
    }
    
private:
    static std::string getExtendedErrorInfo();
    static std::string formatErrorMessage(int code, const std::string &message);
    static void logErrorMessage(int code, const std::string &message);
    
    const int code;
    const std::string message;
    
};


#endif /* !defined(__nidaqtest__NIDAQError__) */




























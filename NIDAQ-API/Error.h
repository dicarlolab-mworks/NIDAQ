//
//  Error.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__Error__
#define __NIDAQ__Error__

#include <stdexcept>
#include <string>

#include "Types.h"


BEGIN_NAMESPACE_NIDAQ


class Error : public std::runtime_error {
    
public:
    static bool failed(int32_t errorCode) {
        return (errorCode != 0);
    }
    
    static void throwIfFailed(int32_t errorCode) {
        if (failed(errorCode)) {
            throw Error(errorCode, getExtendedErrorInfo());
        }
    }
    
    static void logIfFailed(int32_t errorCode) {
        if (failed(errorCode)) {
            logErrorMessage(errorCode, getExtendedErrorInfo());
        }
    }
    
    ~Error() throw() /*override*/ { }
    
    Error(int32_t code, const std::string &message);
    
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


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__Error__) */




























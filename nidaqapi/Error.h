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
            throw Error(getExtendedErrorInfo(), errorCode);
        }
    }
    
    static void logIfFailed(int32_t errorCode) {
        if (failed(errorCode)) {
            logErrorMessage(getExtendedErrorInfo(), errorCode);
        }
    }
    
    ~Error() throw() /*override*/ { }
    
    Error(const std::string &message, int32_t code = 0);
    
    const std::string& getMessage() const {
        return message;
    }
    
    int32_t getCode() const {
        return code;
    }
    
private:
    static std::string getExtendedErrorInfo();
    static std::string formatErrorMessage(const std::string &message, int32_t code);
    static void logErrorMessage(const std::string &message, int32_t code);
    
    const std::string message;
    const int32_t code;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__Error__) */




























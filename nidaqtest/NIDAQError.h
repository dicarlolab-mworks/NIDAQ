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
    static std::string getErrorMessage(int32 errorCode);
    
    static void throwOnFailure(int32 errorCode) {
        if (DAQmxFailed(errorCode)) {
            throw NIDAQError(errorCode);
        }
    }
    
    NIDAQError(int32 errorCode) :
        std::runtime_error(getErrorMessage(errorCode)),
        errorCode(errorCode)
    { }
    
    int32 getErrorCode() const {
        return errorCode;
    }
    
private:
    const int32 errorCode;
    
};


#endif /* !defined(__nidaqtest__NIDAQError__) */

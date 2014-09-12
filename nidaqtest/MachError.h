//
//  MachError.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/10/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__MachError__
#define __NIDAQ__MachError__

#include <stdexcept>

#include <mach/error.h>


class MachError : public std::runtime_error {
    
public:
    static void throwIfFailed(mach_error_t error) {
        if (error != ERR_SUCCESS) {
            throw MachError(error);
        }
    }
    
    explicit MachError(mach_error_t value);
    
    mach_error_t getValue() const {
        return value;
    }
    
private:
    const mach_error_t value;
    
};


#endif /* !defined(__NIDAQ__MachError__) */

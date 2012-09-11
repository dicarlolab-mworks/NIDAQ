//
//  MachClock.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/10/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__MachClock__
#define __NIDAQ__MachClock__

#include <mach/mach_time.h>


class MachClock {
    
public:
    MachClock() :
        absoluteToNano(getAbsoluteToNano())
    {
        reset();
    }
    
    double elapsedNano() const {
        return convertAbsoluteToNano(mach_absolute_time() - startTime);
    }
    
    double elapsedMilli() const {
        return convertNanoToMilli(elapsedNano());
    }
    
    double intervalNano() {
        uint64_t currentTime = mach_absolute_time();
        double interval = convertAbsoluteToNano(currentTime - intervalStartTime);
        intervalStartTime = currentTime;
        return interval;
    }
    
    double intervalMilli() {
        return convertNanoToMilli(intervalNano());
    }
    
    void reset() {
        startTime = mach_absolute_time();
        intervalStartTime = startTime;
    }
    
private:
    static double getAbsoluteToNano();
    
    static double convertNanoToMilli(double value) {
        return value / 1e6;
    }
    
    double convertAbsoluteToNano(uint64_t value) const {
        return double(value) * absoluteToNano;
    }
    
    const double absoluteToNano;
    uint64_t startTime;
    uint64_t intervalStartTime;
    
};


#endif /* !defined(__NIDAQ__MachClock__) */



























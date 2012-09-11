//
//  MachTimer.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/11/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__MachTimer__
#define __NIDAQ__MachTimer__

#include "MachClock.h"


class MachTimer {
    
public:
    MachTimer() :
        absoluteToNano(double(clock.timebaseInfo.numer) / double(clock.timebaseInfo.denom))
    {
        reset();
    }
    
    double elapsedNano() const {
        return convertAbsoluteToNano(clock.absoluteTime() - startTime);
    }
    
    double elapsedMilli() const {
        return convertNanoToMilli(elapsedNano());
    }
    
    double intervalNano() {
        time_type currentTime = clock.absoluteTime();
        double interval = convertAbsoluteToNano(currentTime - intervalStartTime);
        intervalStartTime = currentTime;
        return interval;
    }
    
    double intervalMilli() {
        return convertNanoToMilli(intervalNano());
    }
    
    void reset() {
        startTime = clock.absoluteTime();
        intervalStartTime = startTime;
    }
    
private:
    typedef MachClock::time_type time_type;
    
    static double convertNanoToMilli(double value) {
        return value / 1e6;
    }
    
    double convertAbsoluteToNano(uint64_t value) const {
        return double(value) * absoluteToNano;
    }
    
    MachClock clock;
    const double absoluteToNano;
    time_type startTime;
    time_type intervalStartTime;
    
};


#endif /* !defined(__NIDAQ__MachTimer__) */




























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
    { }
    
    double nanoTime() const {
        return double(mach_absolute_time()) * absoluteToNano;
    }
    
    double milliTime() const {
        return nanoTime() / 1e6;
    }
    
private:
    static double getAbsoluteToNano();
    
    const double absoluteToNano;
    
};


#endif /* !defined(__NIDAQ__MachClock__) */

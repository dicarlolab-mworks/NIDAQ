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
    MachClock();
    
    double nanoTime() const {
        return absoluteTime() * absoluteToNano;
    }
    
    double milliTime() const {
        return absoluteTime() * absoluteToMilli;
    }
    
private:
    static double absoluteTime() {
        return double(mach_absolute_time());
    }
    
    double absoluteToNano;
    double absoluteToMilli;
    
};


#endif /* !defined(__NIDAQ__MachClock__) */

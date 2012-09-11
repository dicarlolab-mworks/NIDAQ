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

#include <boost/cstdint.hpp>


class MachClock {
    
public:
    typedef boost::uint64_t time_type;
    
    static time_type absoluteTime() {
        return time_type(mach_absolute_time());
    }
    
    MachClock() :
        timebaseInfo(getTimebaseInfo())
    { }
    
    time_type nanoTime() const {
        return absoluteTime() * time_type(timebaseInfo.numer) / time_type(timebaseInfo.denom);
    }
    
    const mach_timebase_info_data_t timebaseInfo;
    
private:
    static mach_timebase_info_data_t getTimebaseInfo();
    
};


#endif /* !defined(__NIDAQ__MachClock__) */

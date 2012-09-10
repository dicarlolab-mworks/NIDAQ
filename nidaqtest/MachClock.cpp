//
//  MachClock.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/10/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "MachClock.h"

#include "MachError.h"


double MachClock::getAbsoluteToNano() {
    mach_timebase_info_data_t timebaseInfo;
    MachError::throwIfFailed(  mach_timebase_info(&timebaseInfo)  );
    return (double(timebaseInfo.numer) / double(timebaseInfo.denom));
}

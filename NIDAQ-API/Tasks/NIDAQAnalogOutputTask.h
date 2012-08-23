//
//  NIDAQAnalogOutputTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQAnalogOutputTask__
#define __NIDAQ__NIDAQAnalogOutputTask__

#include "NIDAQTask.h"


class NIDAQAnalogOutputTask : public NIDAQTask {
    
public:
    NIDAQAnalogOutputTask(const NIDAQDevice &device);
    
    void addVoltageChannel(unsigned int channelNumber, double minVal, double maxVal);
    
    int32_t write(const std::vector<double> &samples, double timeout, bool interleaved = false);
    
};


#endif /* !defined(__NIDAQ__NIDAQAnalogOutputTask__) */

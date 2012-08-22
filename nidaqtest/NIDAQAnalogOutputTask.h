//
//  NIDAQAnalogOutputTask.h
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __nidaqtest__NIDAQAnalogOutputTask__
#define __nidaqtest__NIDAQAnalogOutputTask__

#include "NIDAQTask.h"


class NIDAQAnalogOutputTask : public NIDAQTask {
    
public:
    NIDAQAnalogOutputTask(const NIDAQDevice &device);
    
    void addVoltageChannel(unsigned int channelNumber, double minVal, double maxVal);
    
    int32_t write(double timeout, const std::vector<double> &samples, bool interleaved = false);
    
};


#endif /* !defined(__nidaqtest__NIDAQAnalogOutputTask__) */

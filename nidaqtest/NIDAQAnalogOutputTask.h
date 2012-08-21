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
    NIDAQAnalogOutputTask(const NIDAQDevice &device, const std::string &name) :
        NIDAQTask(device, name)
    { }
    
    void createVoltageChannel(const std::string &physicalChannel, double minVal, double maxVal);
    
    int write(int numSampsPerChan, double timeout, bool interleaved, const std::vector<double> &writeArray);
    
};


#endif /* !defined(__nidaqtest__NIDAQAnalogOutputTask__) */

//
//  NIDAQAnalogInputTask.h
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/22/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __nidaqtest__NIDAQAnalogInputTask__
#define __nidaqtest__NIDAQAnalogInputTask__

#include "NIDAQTask.h"


class NIDAQAnalogInputTask : public NIDAQTask {
    
public:
    typedef enum {
        TerminalConfigDefault,
        TerminalConfigRSE,
        TerminalConfigNRSE,
        TerminalConfigDifferential
    } TerminalConfig;
    
    NIDAQAnalogInputTask(const NIDAQDevice &device);
    
    void addVoltageChannel(unsigned int channelNumber,
                           double minVal,
                           double maxVal,
                           TerminalConfig termConfig = TerminalConfigDefault);
    
    int32_t read(double timeout, std::vector<double> &samples, bool interleaved = false);
    
};


#endif /* !defined(__nidaqtest__NIDAQAnalogInputTask__) */

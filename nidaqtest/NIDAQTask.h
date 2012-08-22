//
//  NIDAQTask.h
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __nidaqtest__NIDAQTask__
#define __nidaqtest__NIDAQTask__

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include "NIDAQDevice.h"


class NIDAQTask : boost::noncopyable {
    
public:
    ~NIDAQTask();
    
    NIDAQTask(const NIDAQDevice &device, const std::string &name);
    
    void configureSampleClockTiming(const std::string &source,
                                    double rate,
                                    bool acquireOnRisingEdge,
                                    bool continous,
                                    uint64_t sampsPerChanToAcquire);
    
    void start();
    void stop();
    
protected:
    typedef unsigned long * TaskHandle;
    
    TaskHandle getHandle() const {
        return handle;
    }
    
private:
    TaskHandle handle;
    const std::string deviceName;
    bool running;
    
};


#endif /* !defined(__nidaqtest__NIDAQTask__) */



























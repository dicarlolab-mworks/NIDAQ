//
//  NIDAQTask.h
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __nidaqtest__NIDAQTask__
#define __nidaqtest__NIDAQTask__

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include "NIDAQDevice.h"


class NIDAQTask : boost::noncopyable {
    
public:
    ~NIDAQTask();
    
    void setSampleClockTiming(double samplingRate,
                              const std::string &clockSourceTerminal = "",
                              uint64_t samplesPerChannelToAcquire = 0,
                              bool acquireOnRisingEdge = true);
    
    void start();
    void stop();
    
protected:
    typedef unsigned long * TaskHandle;
    
    NIDAQTask(const NIDAQDevice &device, const std::string &name);
    
    TaskHandle getHandle() const {
        return handle;
    }
    
    const std::string& getDeviceName() const {
        return deviceName;
    }
    
    size_t getNumChannels() const {
        return numChannels;
    }
    
    void addChannel() {
        numChannels++;
    }
    
    template <typename T>
    int32_t getNumSamplesPerChannel(const std::vector<T> &samples) const {
        if ((getNumChannels() == 0) || (samples.size() % getNumChannels() != 0))
        {
            throw std::invalid_argument("Invalid number of samples");
        }
        return samples.size() / getNumChannels();
    }
    
private:
    TaskHandle handle;
    const std::string deviceName;
    size_t numChannels;
    bool running;
    
};


#endif /* !defined(__nidaqtest__NIDAQTask__) */



























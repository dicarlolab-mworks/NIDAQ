//
//  Task.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__Task__
#define __NIDAQ__Task__

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include "Device.h"


BEGIN_NAMESPACE_NIDAQ


class Task : boost::noncopyable {
    
public:
    ~Task();
    
    void setSampleClockTiming(double samplingRate,
                              const std::string &clockSourceTerminal = "",
                              uint64_t samplesPerChannelToAcquire = 0,
                              bool acquireOnRisingEdge = true);
    
    void setAllowRegeneration(bool allowRegen);
    
    void start();
    void stop();
    
    bool isRunning() const {
        return running;
    }
    
protected:
    typedef unsigned long * TaskHandle;
    
    Task(const Device &device, const std::string &name);
    
    TaskHandle getHandle() const {
        return handle;
    }
    
    const std::string& getDeviceName() const {
        return deviceName;
    }
    
    void addChannel(const std::string &name) {
        channels.push_back(name);
    }
    
    template <typename T>
    int32_t getNumSamplesPerChannel(const std::vector<T> &samples) const {
        if ((channels.size() == 0) || (samples.size() % channels.size() != 0))
        {
            throw std::invalid_argument("Invalid number of samples");
        }
        return samples.size() / channels.size();
    }
    
private:
    TaskHandle handle;
    const std::string deviceName;
    std::vector<std::string> channels;
    bool running;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__Task__) */



























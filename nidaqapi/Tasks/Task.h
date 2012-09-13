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

#include <boost/noncopyable.hpp>

#include "Device.h"


BEGIN_NAMESPACE_NIDAQ


class Task : boost::noncopyable {
    
public:
    ~Task();
    
    void setSampleClockTiming(double samplingRate,
                              uint64_t samplesPerChannelToAcquire,
                              bool continuous = true,
                              const std::string &clockSourceTerminal = "",
                              bool acquireOnRisingEdge = true);
    
    void start();
    void stop();
    
    size_t getNumChannels() const {
        return numChannels;
    }
    
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
    
    std::string getChannelName(const std::string &type, unsigned int number) const;
    
    void addChannel() {
        numChannels++;
    }
    
    int32_t getNumSamplesPerChannel(size_t numSamples) const {
        if ((numChannels == 0) || (numSamples % numChannels != 0))
        {
            throw std::invalid_argument("Invalid number of samples");
        }
        return numSamples / numChannels;
    }
    
private:
    TaskHandle handle;
    const std::string deviceName;
    size_t numChannels;
    bool running;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__Task__) */



























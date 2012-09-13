//
//  Task.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__Task__
#define __NIDAQ__Task__

#include <set>
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
    
    bool isRunning() const {
        return running;
    }
    
protected:
    typedef unsigned long * TaskHandle;
    
    Task(const Device &device, const std::string &taskType);
    
    TaskHandle getHandle() const {
        return handle;
    }
    
    const std::string& getDeviceName() const {
        return deviceName;
    }
    
    std::string getChannelName(const std::string &type, unsigned int number) const;
    void addChannel(const std::string &name);
    
    size_t getNumChannels() const {
        return channelNames.size();
    }
    
    int32_t getNumSamplesPerChannel(size_t numSamples) const {
        size_t numChannels = getNumChannels();
        if ((numChannels == 0) || (numSamples % numChannels != 0))
        {
            throw std::invalid_argument("Invalid number of samples");
        }
        return numSamples / numChannels;
    }
    
private:
    static std::set<std::string> allTaskNames;
    static std::set<std::string> allChannelNames;
    
    TaskHandle handle;
    const std::string deviceName;
    const std::string taskName;
    std::set<std::string> channelNames;
    bool running;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__Task__) */



























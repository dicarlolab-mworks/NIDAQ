//
//  Task.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__Task__
#define __NIDAQ__Task__

#include <string>

#include <boost/noncopyable.hpp>

#include "Types.h"


BEGIN_NAMESPACE_NIDAQ


class Device;  // Forward declaration


class Task : boost::noncopyable {
    
public:
    ~Task();
    
    void setSampleClockTiming(double samplingRate,
                              std::uint64_t samplesPerChannelToAcquire,
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
    
    explicit Task(Device &device);
    
    TaskHandle getHandle() const {
        return handle;
    }
    
    std::string getChannelName(const std::string &type, unsigned int number) const;
    void addChannel(const std::string &name);
    
    std::size_t getNumChannels() const {
        return numChannels;
    }
    
    std::int32_t getNumSamplesPerChannel(std::size_t numSamples) const;
    
private:
    Device &device;
    TaskHandle handle;
    std::size_t numChannels;
    bool running;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__Task__) */



























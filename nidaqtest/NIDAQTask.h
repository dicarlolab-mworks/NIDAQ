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

#include <NIDAQmxBase.h>


class NIDAQDevice;  // Forward declaration


class NIDAQTask : boost::noncopyable {
    
public:
    virtual ~NIDAQTask();
    
    NIDAQTask(const NIDAQDevice &device, const std::string &name);
    
    void createAnalogOutputVoltageChannel(const std::string &physicalChannel, float64 minVal, float64 maxVal);
    
    void configureSampleClockTiming(const std::string &source,
                                    float64 rate,
                                    int32 activeEdge,
                                    int32 sampleMode,
                                    uInt64 sampsPerChanToAcquire);
    
    int32 writeAnalog(int32 numSampsPerChan,
                      float64 timeout,
                      bool32 dataLayout,
                      const std::vector<float64> &writeArray);
    
    void start();
    void stop();
    
private:
    TaskHandle handle;
    const std::string deviceName;
    bool running;
    
};


#endif /* !defined(__nidaqtest__NIDAQTask__) */

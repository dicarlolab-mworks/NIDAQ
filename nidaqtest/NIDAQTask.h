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


class NIDAQDevice;  // Forward declaration


class NIDAQTask : boost::noncopyable {
    
public:
    virtual ~NIDAQTask();
    
    NIDAQTask(const NIDAQDevice &device, const std::string &name);
    
    void createAnalogOutputVoltageChannel(const std::string &physicalChannel, double minVal, double maxVal);
    
    void configureSampleClockTiming(const std::string &source,
                                    double rate,
                                    bool acquireOnRisingEdge,
                                    bool continous,
                                    unsigned long long sampsPerChanToAcquire);
    
    int writeAnalog(int numSampsPerChan,
                    double timeout,
                    bool interleaved,
                    const std::vector<double> &writeArray);
    
    void start();
    void stop();
    
private:
    void *handle;
    const std::string deviceName;
    bool running;
    
};


#endif /* !defined(__nidaqtest__NIDAQTask__) */



























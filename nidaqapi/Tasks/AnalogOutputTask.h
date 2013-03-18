//
//  AnalogOutputTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__AnalogOutputTask__
#define __NIDAQ__AnalogOutputTask__

#include "Task.h"


BEGIN_NAMESPACE_NIDAQ


class Device::AnalogOutputTask : public Device::Task {
    
public:
    explicit AnalogOutputTask(const Device &device);
    
    void setAllowRegeneration(bool allowRegen);
    
    void addVoltageChannel(unsigned int channelNumber, double minVal, double maxVal);
    
    template <typename DoubleArrayType>
    std::size_t write(const DoubleArrayType &samples, double timeout, bool interleaved = false) {
        return write(samples[0], samples.size(), timeout, interleaved);
    }
    
private:
    std::size_t write(const double &firstSample, std::size_t numSamples, double timeout, bool interleaved);
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__AnalogOutputTask__) */

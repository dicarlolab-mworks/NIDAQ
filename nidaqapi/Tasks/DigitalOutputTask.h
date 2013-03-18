//
//  DigitalOutputTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/12/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__DigitalOutputTask__
#define __NIDAQ__DigitalOutputTask__

#include "Task.h"


BEGIN_NAMESPACE_NIDAQ


class Device::DigitalOutputTask : public Device::Task {
    
public:
    explicit DigitalOutputTask(Device &device);
    
    void addChannel(unsigned int portNumber);
    
    template <typename UInt32ArrayType>
    std::size_t write(const UInt32ArrayType &samples, double timeout, bool interleaved = false) {
        return write(samples[0], samples.size(), timeout, interleaved);
    }
    
private:
    std::size_t write(const std::uint32_t &firstSample, std::size_t numSamples, double timeout, bool interleaved);
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__DigitalOutputTask__) */

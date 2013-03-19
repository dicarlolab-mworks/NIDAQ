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


class DigitalOutputTask : public Task {
    
public:
    std::size_t write(std::uint32_t sample, double timeout) {
        return write(sample, 1, timeout);
    }
    
    template <typename UInt32ArrayType>
    std::size_t write(const UInt32ArrayType &samples, double timeout) {
        return write(samples[0], samples.size(), timeout);
    }
    
private:
    DigitalOutputTask(Device &device, unsigned int portNumber);
    
    std::size_t write(const std::uint32_t &firstSample, std::size_t numSamples, double timeout);
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__DigitalOutputTask__) */

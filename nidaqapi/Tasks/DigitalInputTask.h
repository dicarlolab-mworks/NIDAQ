//
//  DigitalInputTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/12/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__DigitalInputTask__
#define __NIDAQ__DigitalInputTask__

#include "Task.h"


BEGIN_NAMESPACE_NIDAQ


class DigitalInputTask : public Task {
    
public:
    void addChannel(unsigned int portNumber);
    
    template <typename UInt32ArrayType>
    std::size_t read(UInt32ArrayType &samples, double timeout, bool interleaved = false) {
        return read(samples[0], samples.size(), timeout, interleaved);
    }
    
private:
    explicit DigitalInputTask(Device &device);
    
    std::size_t read(std::uint32_t &firstSample, std::size_t numSamples, double timeout, bool interleaved);
    
    friend class Device;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__DigitalInputTask__) */

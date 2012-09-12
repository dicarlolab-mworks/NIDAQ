//
//  DigitalOutputTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/12/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__DigitalOutputTask__
#define __NIDAQ__DigitalOutputTask__

#include "DigitalIOTask.h"


BEGIN_NAMESPACE_NIDAQ


class DigitalOutputTask : public DigitalIOTask {
    
public:
    explicit DigitalOutputTask(const Device &device);
    
    template <typename UInt32ArrayType>
    size_t write(const UInt32ArrayType &samples, double timeout, bool interleaved = false) {
        return write(samples[0], samples.size(), timeout, interleaved);
    }
    
private:
    int32_t createChannel(const std::string &lines) /*override*/;
    
    size_t write(const uint32_t &firstSample, size_t numSamples, double timeout, bool interleaved);
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__DigitalOutputTask__) */

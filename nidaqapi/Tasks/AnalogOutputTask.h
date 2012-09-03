//
//  AnalogOutputTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__AnalogOutputTask__
#define __NIDAQ__AnalogOutputTask__

#include <boost/array.hpp>

#include "Task.h"


BEGIN_NAMESPACE_NIDAQ


class AnalogOutputTask : public Task {
    
public:
    AnalogOutputTask(const Device &device);
    
    void addVoltageChannel(unsigned int channelNumber, double minVal, double maxVal);
    
    int32_t write(const std::vector<double> &samples, double timeout, bool interleaved = false) {
        return write(samples.front(), samples.size(), timeout, interleaved);
    }
    
    template <std::size_t numSamples>
    int32_t write(const boost::array<double, numSamples> &samples, double timeout, bool interleaved = false) {
        return write(samples.front(), numSamples, timeout, interleaved);
    }
    
private:
    int32_t write(const double &firstSample, std::size_t numSamples, double timeout, bool interleaved);
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__AnalogOutputTask__) */

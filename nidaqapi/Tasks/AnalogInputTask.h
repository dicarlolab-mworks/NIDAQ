//
//  AnalogInputTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/22/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__AnalogInputTask__
#define __NIDAQ__AnalogInputTask__

#include <boost/array.hpp>

#include "Task.h"


BEGIN_NAMESPACE_NIDAQ


class AnalogInputTask : public Task {
    
public:
    typedef enum {
        TerminalConfigDefault,
        TerminalConfigRSE,
        TerminalConfigNRSE,
        TerminalConfigDifferential
    } TerminalConfig;
    
    AnalogInputTask(const Device &device);
    
    void addVoltageChannel(unsigned int channelNumber,
                           double minVal,
                           double maxVal,
                           TerminalConfig termConfig = TerminalConfigDefault);
    
    int32_t read(std::vector<double> &samples, double timeout, bool interleaved = false) {
        return read(samples.front(), samples.size(), timeout, interleaved);
    }
    
    template <std::size_t numSamples>
    int32_t read(boost::array<double, numSamples> &samples, double timeout, bool interleaved = false) {
        return read(samples.front(), numSamples, timeout, interleaved);
    }
    
private:
    static int32_t getTerminalConfigValue(TerminalConfig termConfig);
    
    int32_t read(double &firstSample, std::size_t numSamples, double timeout, bool interleaved);
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__AnalogInputTask__) */



























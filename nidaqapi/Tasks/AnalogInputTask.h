//
//  AnalogInputTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/22/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__AnalogInputTask__
#define __NIDAQ__AnalogInputTask__

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
    
    explicit AnalogInputTask(const Device &device);
    
    void addVoltageChannel(unsigned int channelNumber,
                           double minVal,
                           double maxVal,
                           TerminalConfig termConfig = TerminalConfigDefault);
    
    std::size_t getNumSamplesAvailable() const;
    
    template <typename DoubleArrayType>
    std::size_t read(DoubleArrayType &samples, double timeout, bool interleaved = false) {
        return read(samples[0], samples.size(), timeout, interleaved);
    }
    
private:
    static std::int32_t getTerminalConfigValue(TerminalConfig termConfig);
    
    std::size_t read(double &firstSample, std::size_t numSamples, double timeout, bool interleaved);
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__AnalogInputTask__) */



























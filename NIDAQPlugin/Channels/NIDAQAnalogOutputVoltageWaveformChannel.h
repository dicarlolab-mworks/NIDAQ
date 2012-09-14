//
//  NIDAQAnalogOutputVoltageWaveformChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/13/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQAnalogOutputVoltageWaveformChannel__
#define __NIDAQ__NIDAQAnalogOutputVoltageWaveformChannel__

#include <boost/function.hpp>

#include "NIDAQAnalogChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQAnalogOutputVoltageWaveformChannel : public NIDAQAnalogChannel {
    
public:
    static const std::string WAVEFORM;
    static const std::string PERIOD;
    static const std::string OFFSET;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQAnalogOutputVoltageWaveformChannel(const ParameterValueMap &parameters);
    
    double getSampleForTime(double time) const;
    
private:
    typedef boost::function<double (double period, double time)> WaveformFunction;
    
    static double sinusoid(double period, double time);
    
    WaveformFunction waveform;
    double amplitude;
    double period;
    double timeOffset;
    double voltageOffset;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQAnalogOutputVoltageWaveformChannel__) */



























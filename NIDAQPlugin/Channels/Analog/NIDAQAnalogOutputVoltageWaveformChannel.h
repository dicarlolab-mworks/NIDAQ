//
//  NIDAQAnalogOutputVoltageWaveformChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/13/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQAnalogOutputVoltageWaveformChannel__
#define __NIDAQ__NIDAQAnalogOutputVoltageWaveformChannel__

#include "NIDAQAnalogChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQAnalogOutputVoltageWaveformChannel : public NIDAQAnalogChannel {
    
public:
    static const std::string WAVEFORM;
    static const std::string PERIOD;
    static const std::string OFFSET;
    static const std::string AMPLITUDE;
    static const std::string MEAN;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQAnalogOutputVoltageWaveformChannel(const ParameterValueMap &parameters);
    
    std::string getWaveform() const {
        return waveform->getValue().getString();
    }
    
    MWTime getPeriod() const {
        return period->getValue().getInteger();
    }
    
    MWTime getTimeOffset() const {
        return timeOffset->getValue().getInteger();
    }
    
    double getAmplitude() const {
        if (amplitude) {
            return amplitude->getValue().getFloat();
        }
        return (getRangeMax() - getRangeMin()) / 2.0;
    }
    
    double getVoltageOffset() const {
        if (voltageOffset) {
            return voltageOffset->getValue().getFloat();
        }
        return (getRangeMin() + getRangeMax()) / 2.0;
    }
    
    double getSampleForTime(MWTime time) const {
        return ((getAmplitude()
                 * getWaveformFunction(getWaveform())(double(getPeriod()), double(time + getTimeOffset())))
                + getVoltageOffset());
    }
    
private:
    using WaveformFunction = double (*)(double period, double time);
    static WaveformFunction getWaveformFunction(std::string waveform);
    
    static double sinusoid(double period, double time);
    static double square(double period, double time);
    static double triangle(double period, double time);
    static double sawtooth(double period, double time);
    static double null(double period, double time);
    
    const VariablePtr waveform;
    const VariablePtr period;
    const VariablePtr timeOffset;
    const VariablePtr amplitude;
    const VariablePtr voltageOffset;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQAnalogOutputVoltageWaveformChannel__) */



























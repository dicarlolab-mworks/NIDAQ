//
//  NIDAQAnalogOutputVoltageWaveformChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/13/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQAnalogOutputVoltageWaveformChannel.h"

#include <cmath>

#include <boost/algorithm/string.hpp>


BEGIN_NAMESPACE_MW


const std::string NIDAQAnalogOutputVoltageWaveformChannel::WAVEFORM("waveform");
const std::string NIDAQAnalogOutputVoltageWaveformChannel::PERIOD("period");
const std::string NIDAQAnalogOutputVoltageWaveformChannel::OFFSET("offset");


void NIDAQAnalogOutputVoltageWaveformChannel::describeComponent(ComponentInfo &info) {
    NIDAQAnalogChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_analog_output_voltage_waveform");
    info.addParameter(WAVEFORM);
    info.addParameter(PERIOD);
    info.addParameter(OFFSET);
}


NIDAQAnalogOutputVoltageWaveformChannel::NIDAQAnalogOutputVoltageWaveformChannel(const ParameterValueMap &parameters) :
    NIDAQAnalogChannel(parameters),
    amplitude((getRangeMax() - getRangeMin()) / 2.0),
    period(parameters[PERIOD]),
    timeOffset(parameters[OFFSET]),
    voltageOffset(getRangeMin() + amplitude)
{
    std::string waveformType(boost::algorithm::to_lower_copy(parameters[WAVEFORM].str()));
    if (waveformType == "sinusoid") {
        waveform = &sinusoid;
    } else {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid waveform type", waveformType);
    }
}


double NIDAQAnalogOutputVoltageWaveformChannel::getSampleForTime(double time) const {
    return amplitude * waveform(period, time - timeOffset) + voltageOffset;
}


double NIDAQAnalogOutputVoltageWaveformChannel::sinusoid(double period, double time) {
    return std::sin((2.0*M_PI / period) * time);
}


END_NAMESPACE_MW



























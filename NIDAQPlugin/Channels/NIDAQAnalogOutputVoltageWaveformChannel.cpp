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
    waveformFunc(getWaveformFunction(parameters[WAVEFORM].str())),
    amplitude((getRangeMax() - getRangeMin()) / 2.0),
    period(parameters[PERIOD]),
    timeOffset(parameters[OFFSET]),
    voltageOffset(getRangeMin() + amplitude)
{ }


NIDAQAnalogOutputVoltageWaveformChannel::WaveformFunction
NIDAQAnalogOutputVoltageWaveformChannel::getWaveformFunction(std::string waveform)
{
    boost::algorithm::to_lower(waveform);
    if (waveform == "sinusoid") {
        return &sinusoid;
    } else {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid waveform type", waveform);
    }
}


double NIDAQAnalogOutputVoltageWaveformChannel::sinusoid(double period, double time) {
    return std::sin((2.0*M_PI / period) * time);
}


END_NAMESPACE_MW



























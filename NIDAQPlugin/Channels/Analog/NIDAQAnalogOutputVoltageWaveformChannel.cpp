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
const std::string NIDAQAnalogOutputVoltageWaveformChannel::AMPLITUDE("amplitude");
const std::string NIDAQAnalogOutputVoltageWaveformChannel::MEAN("mean");


void NIDAQAnalogOutputVoltageWaveformChannel::describeComponent(ComponentInfo &info) {
    NIDAQAnalogChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_analog_output_voltage_waveform");
    info.addParameter(WAVEFORM);
    info.addParameter(PERIOD);
    info.addParameter(OFFSET, "0ms");
    info.addParameter(AMPLITUDE, false);
    info.addParameter(MEAN, false);
}


NIDAQAnalogOutputVoltageWaveformChannel::NIDAQAnalogOutputVoltageWaveformChannel(const ParameterValueMap &parameters) :
    NIDAQAnalogChannel(parameters),
    waveform(variableOrText(parameters[WAVEFORM])),
    period(parameters[PERIOD]),
    timeOffset(parameters[OFFSET]),
    amplitude(optionalVariable(parameters[AMPLITUDE])),
    voltageOffset(optionalVariable(parameters[MEAN]))
{ }


auto NIDAQAnalogOutputVoltageWaveformChannel::getWaveformFunction(std::string waveform) -> WaveformFunction {
    boost::algorithm::to_lower(waveform);
    if (waveform == "sinusoid") {
        return &sinusoid;
    } else if (waveform == "square") {
        return &square;
    } else if (waveform == "triangle") {
        return &triangle;
    } else if (waveform == "sawtooth") {
        return &sawtooth;
    }
    merror(M_IODEVICE_MESSAGE_DOMAIN,
           "Invalid waveform type (%s) for NIDAQ analog output waveform channel",
           waveform.c_str());
    return &null;
}


double NIDAQAnalogOutputVoltageWaveformChannel::sinusoid(double period, double time) {
    return std::sin((2.0*M_PI / period) * time);
}


double NIDAQAnalogOutputVoltageWaveformChannel::square(double period, double time) {
    return std::pow(-1.0, std::floor(2.0 * time / period));
}


double NIDAQAnalogOutputVoltageWaveformChannel::triangle(double period, double time) {
    return M_2_PI * std::asin(sinusoid(period, time));
}


double NIDAQAnalogOutputVoltageWaveformChannel::sawtooth(double period, double time) {
    return 2.0 * (time/period - std::floor(0.5 + time/period));
}


double NIDAQAnalogOutputVoltageWaveformChannel::null(double period, double time) {
    return 0.0;
}


END_NAMESPACE_MW



























//
//  NIDAQAnalogOutputVoltageChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/12/14.
//  Copyright (c) 2014 MIT. All rights reserved.
//

#include "NIDAQAnalogOutputVoltageChannel.h"


BEGIN_NAMESPACE_MW


void NIDAQAnalogOutputVoltageChannel::describeComponent(ComponentInfo &info) {
    NIDAQAnalogChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_analog_output_voltage");
    info.addParameter(VOLTAGE);
}


NIDAQAnalogOutputVoltageChannel::NIDAQAnalogOutputVoltageChannel(const ParameterValueMap &parameters) :
    NIDAQAnalogChannel(parameters),
    voltage(parameters[VOLTAGE])
{ }


END_NAMESPACE_MW




























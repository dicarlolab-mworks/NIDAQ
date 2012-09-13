//
//  NIDAQAnalogInputVoltageChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/3/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQAnalogInputVoltageChannel.h"


BEGIN_NAMESPACE_MW


const std::string NIDAQAnalogInputVoltageChannel::VARIABLE("variable");


void NIDAQAnalogInputVoltageChannel::describeComponent(ComponentInfo &info) {
    NIDAQAnalogChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_analog_input_voltage");
    info.addParameter(VARIABLE);
}


NIDAQAnalogInputVoltageChannel::NIDAQAnalogInputVoltageChannel(const ParameterValueMap &parameters) :
    NIDAQAnalogChannel(parameters),
    variable(parameters[VARIABLE])
{ }


END_NAMESPACE_MW

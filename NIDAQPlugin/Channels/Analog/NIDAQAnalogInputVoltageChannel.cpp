//
//  NIDAQAnalogInputVoltageChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/3/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQAnalogInputVoltageChannel.h"


BEGIN_NAMESPACE_MW


void NIDAQAnalogInputVoltageChannel::describeComponent(ComponentInfo &info) {
    NIDAQAnalogChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_analog_input_voltage");
    info.addParameter(VOLTAGE);
}


NIDAQAnalogInputVoltageChannel::NIDAQAnalogInputVoltageChannel(const ParameterValueMap &parameters) :
    NIDAQAnalogChannel(parameters),
    voltage(parameters[VOLTAGE])
{ }


END_NAMESPACE_MW

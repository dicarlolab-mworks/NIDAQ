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
    NIDAQAnalogInputChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_analog_input_voltage");
}


NIDAQAnalogInputVoltageChannel::NIDAQAnalogInputVoltageChannel(const ParameterValueMap &parameters) :
    NIDAQAnalogInputChannel(parameters)
{ }


END_NAMESPACE_MW

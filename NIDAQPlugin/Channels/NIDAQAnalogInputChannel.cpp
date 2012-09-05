//
//  NIDAQAnalogInputChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/3/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQAnalogInputChannel.h"


BEGIN_NAMESPACE_MW


const std::string NIDAQAnalogInputChannel::VARIABLE("variable");


void NIDAQAnalogInputChannel::describeComponent(ComponentInfo &info) {
    NIDAQAnalogChannel::describeComponent(info);
    info.addParameter(VARIABLE);
}


NIDAQAnalogInputChannel::NIDAQAnalogInputChannel(const ParameterValueMap &parameters) :
    NIDAQAnalogChannel(parameters),
    variable(parameters[VARIABLE])
{ }


END_NAMESPACE_MW

//
//  NIDAQAnalogInputChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/3/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQAnalogInputChannel.h"


BEGIN_NAMESPACE_MW


const std::string NIDAQAnalogInputChannel::CHANNEL_NUMBER("channel_number");
const std::string NIDAQAnalogInputChannel::RANGE_MIN("range_min");
const std::string NIDAQAnalogInputChannel::RANGE_MAX("range_max");
const std::string NIDAQAnalogInputChannel::VARIABLE("variable");


void NIDAQAnalogInputChannel::describeComponent(ComponentInfo &info) {
    Component::describeComponent(info);
    info.addParameter(CHANNEL_NUMBER, true, "0");
    info.addParameter(RANGE_MIN, true, "-10.0");
    info.addParameter(RANGE_MAX, true, "10.0");
    info.addParameter(VARIABLE);
}


NIDAQAnalogInputChannel::NIDAQAnalogInputChannel(const ParameterValueMap &parameters) :
    Component(parameters),
    channelNumber(int(parameters[CHANNEL_NUMBER])),
    rangeMin(parameters[RANGE_MIN]),
    rangeMax(parameters[RANGE_MAX]),
    variable(parameters[VARIABLE])
{ }


END_NAMESPACE_MW

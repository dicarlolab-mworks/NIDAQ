//
//  NIDAQAnalogChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/5/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQAnalogChannel.h"


BEGIN_NAMESPACE_MW


const std::string NIDAQAnalogChannel::CHANNEL_NUMBER("channel_number");
const std::string NIDAQAnalogChannel::RANGE_MIN("range_min");
const std::string NIDAQAnalogChannel::RANGE_MAX("range_max");


void NIDAQAnalogChannel::describeComponent(ComponentInfo &info) {
    Component::describeComponent(info);
    info.addParameter(CHANNEL_NUMBER, true, "0");
    info.addParameter(RANGE_MIN, true, "-10.0");
    info.addParameter(RANGE_MAX, true, "10.0");
}


NIDAQAnalogChannel::NIDAQAnalogChannel(const ParameterValueMap &parameters) :
    Component(parameters),
    channelNumber(parameters[CHANNEL_NUMBER]),
    rangeMin(parameters[RANGE_MIN]),
    rangeMax(parameters[RANGE_MAX])
{
    if (channelNumber < 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid channel number");
    }
}


END_NAMESPACE_MW

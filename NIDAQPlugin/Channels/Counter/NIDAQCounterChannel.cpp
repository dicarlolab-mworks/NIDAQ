//
//  NIDAQCounterChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 1/17/14.
//  Copyright (c) 2014 MIT. All rights reserved.
//

#include "NIDAQCounterChannel.h"


BEGIN_NAMESPACE_MW


const std::string NIDAQCounterChannel::COUNTER_NUMBER("counter_number");


void NIDAQCounterChannel::describeComponent(ComponentInfo &info) {
    Component::describeComponent(info);
    info.addParameter(COUNTER_NUMBER, true, "0");
}


NIDAQCounterChannel::NIDAQCounterChannel(const ParameterValueMap &parameters) :
    Component(parameters),
    counterNumber(parameters[COUNTER_NUMBER])
{
    if (counterNumber < 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid counter number");
    }
}


END_NAMESPACE_MW

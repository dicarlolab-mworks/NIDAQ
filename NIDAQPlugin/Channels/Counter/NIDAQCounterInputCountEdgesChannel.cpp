//
//  NIDAQCounterInputCountEdgesChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 1/17/14.
//  Copyright (c) 2014 MIT. All rights reserved.
//

#include "NIDAQCounterInputCountEdgesChannel.h"


BEGIN_NAMESPACE_MW


const std::string NIDAQCounterInputCountEdgesChannel::COUNT("count");


void NIDAQCounterInputCountEdgesChannel::describeComponent(ComponentInfo &info) {
    NIDAQCounterChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_counter_input_count_edges");
    info.addParameter(COUNT);
}


NIDAQCounterInputCountEdgesChannel::NIDAQCounterInputCountEdgesChannel(const ParameterValueMap &parameters) :
    NIDAQCounterChannel(parameters),
    count(parameters[COUNT])
{ }


void NIDAQCounterInputCountEdgesChannel::postCount(std::uint32_t value, MWTime time) const {
    if (count->getValue().getInteger() != value) {
        count->setValue(long(value), time);
    }
}


END_NAMESPACE_MW

//
//  NIDAQDigitalInputChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 3/13/13.
//  Copyright (c) 2013 MIT. All rights reserved.
//

#include "NIDAQDigitalInputChannel.h"

#include <boost/foreach.hpp>


BEGIN_NAMESPACE_MW


void NIDAQDigitalInputChannel::describeComponent(ComponentInfo &info) {
    NIDAQDigitalChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_digital_input");
}


NIDAQDigitalInputChannel::NIDAQDigitalInputChannel(const ParameterValueMap &parameters) :
    NIDAQDigitalChannel(parameters)
{ }


void NIDAQDigitalInputChannel::postSample(boost::uint32_t value, MWTime time) const {
    BOOST_FOREACH(const VariablePtr &variable, getLineVariables()) {
        if (variable) {
            long newVal = long(value & 1u);
            if (variable->getValue().getInteger() != newVal) {
                variable->setValue(newVal, time);
            }
        }
        value >>= 1;
    }
}


END_NAMESPACE_MW

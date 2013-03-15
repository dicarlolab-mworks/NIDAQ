//
//  NIDAQDigitalOutputChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 3/14/13.
//  Copyright (c) 2013 MIT. All rights reserved.
//

#include "NIDAQDigitalOutputChannel.h"

#include <boost/foreach.hpp>


BEGIN_NAMESPACE_MW


void NIDAQDigitalOutputChannel::describeComponent(ComponentInfo &info) {
    NIDAQDigitalChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_digital_output");
}


NIDAQDigitalOutputChannel::NIDAQDigitalOutputChannel(const ParameterValueMap &parameters) :
    NIDAQDigitalChannel(parameters)
{ }


void NIDAQDigitalOutputChannel::addNewSampleNotification(const boost::shared_ptr<VariableNotification> &vn) const {
    BOOST_FOREACH(const VariablePtr &variable, getLineVariables()) {
        if (variable) {
            variable->addNotification(vn);
        }
    }
}


boost::uint32_t NIDAQDigitalOutputChannel::getSample() const {
    boost::uint32_t value = 0;
    
    BOOST_REVERSE_FOREACH(const VariablePtr &variable, getLineVariables()) {
        value <<= 1;
        if (variable && variable->getValue().getBool()) {
            value |= 1u;
        }
    }
    
    return value;
}


END_NAMESPACE_MW




























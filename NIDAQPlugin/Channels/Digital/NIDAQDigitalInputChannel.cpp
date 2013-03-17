//
//  NIDAQDigitalInputChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 3/13/13.
//  Copyright (c) 2013 MIT. All rights reserved.
//

#include "NIDAQDigitalInputChannel.h"


BEGIN_NAMESPACE_MW


void NIDAQDigitalInputChannel::describeComponent(ComponentInfo &info) {
    NIDAQDigitalChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_digital_input");
}


NIDAQDigitalInputChannel::NIDAQDigitalInputChannel(const ParameterValueMap &parameters) :
    NIDAQDigitalChannel(parameters)
{ }


void NIDAQDigitalInputChannel::postLineState(std::size_t lineNumber, bool state, MWTime time) const {
    const VariablePtr &variable = getLineVariable(lineNumber);
    if (variable && (variable->getValue().getBool() != state)) {
        variable->setValue(long(state), time);
    }
}


END_NAMESPACE_MW

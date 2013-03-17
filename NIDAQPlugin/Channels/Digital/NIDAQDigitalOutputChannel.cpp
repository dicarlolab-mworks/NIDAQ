//
//  NIDAQDigitalOutputChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 3/14/13.
//  Copyright (c) 2013 MIT. All rights reserved.
//

#include "NIDAQDigitalOutputChannel.h"


BEGIN_NAMESPACE_MW


void NIDAQDigitalOutputChannel::describeComponent(ComponentInfo &info) {
    NIDAQDigitalChannel::describeComponent(info);
    info.setSignature("iochannel/nidaq_digital_output");
}


NIDAQDigitalOutputChannel::NIDAQDigitalOutputChannel(const ParameterValueMap &parameters) :
    NIDAQDigitalChannel(parameters)
{ }


void NIDAQDigitalOutputChannel::addNewLineStateNotification(std::size_t lineNumber,
                                                            const boost::shared_ptr<VariableNotification> &vn) const
{
    const VariablePtr &variable = getLineVariable(lineNumber);
    if (variable) {
        variable->addNotification(vn);
    }
}


bool NIDAQDigitalOutputChannel::getLineState(std::size_t lineNumber) const {
    const VariablePtr &variable = getLineVariable(lineNumber);
    if (variable) {
        return variable->getValue().getBool();
    }
    return false;
}


END_NAMESPACE_MW




























//
//  NIDAQDigitalChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 3/13/13.
//  Copyright (c) 2013 MIT. All rights reserved.
//

#include "NIDAQDigitalChannel.h"

#include <boost/format.hpp>


BEGIN_NAMESPACE_MW


const std::string NIDAQDigitalChannel::PORT_NUMBER("port_number");


void NIDAQDigitalChannel::describeComponent(ComponentInfo &info) {
    Component::describeComponent(info);
    info.addParameter(PORT_NUMBER, true, "0");
    for (std::size_t i = 0; i < maxNumLines; i++) {
        info.addParameter(getLineParameterName(i), false);
    }
}


NIDAQDigitalChannel::NIDAQDigitalChannel(const ParameterValueMap &parameters) :
    Component(parameters),
    portNumber(parameters[PORT_NUMBER])
{
    if (portNumber < 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid port number");
    }
    for (std::size_t i = 0; i < maxNumLines; i++) {
        const ParameterValue &lineParameter = parameters[getLineParameterName(i)];
        if (!lineParameter.empty()) {
            lineVariables.at(i) = VariablePtr(lineParameter);
        }
    }
}


inline std::string NIDAQDigitalChannel::getLineParameterName(std::size_t lineNumber) {
    return (boost::format("line%d") % lineNumber).str();
}


END_NAMESPACE_MW



























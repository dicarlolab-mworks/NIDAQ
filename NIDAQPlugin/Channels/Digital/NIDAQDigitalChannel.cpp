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
const std::string NIDAQDigitalChannel::NUM_LINES_IN_PORT("num_lines_in_port");


void NIDAQDigitalChannel::describeComponent(ComponentInfo &info) {
    Component::describeComponent(info);
    info.addParameter(PORT_NUMBER, true, "0");
    info.addParameter(NUM_LINES_IN_PORT, true, "8");
    for (std::size_t i = 0; i < maxNumLines; i++) {
        info.addParameter(getLineParameterName(i), false);
    }
}


NIDAQDigitalChannel::NIDAQDigitalChannel(const ParameterValueMap &parameters) :
    Component(parameters),
    portNumber(parameters[PORT_NUMBER]),
    numLinesInPort(parameters[NUM_LINES_IN_PORT])
{
    if (portNumber < 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid port number");
    }
    
    if (numLinesInPort < 1) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid number of lines in port");
    }
    if (numLinesInPort % 4 != 0) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
                 "Reported number of lines (%d) in port %d of NIDAQ device is not a multiple of 4",
                 numLinesInPort,
                 portNumber);
    }
    
    bool foundLineParameter = false;
    for (std::size_t lineNumber = 0; lineNumber < maxNumLines; lineNumber++) {
        const ParameterValue &lineParameter = parameters[getLineParameterName(lineNumber)];
        if (!lineParameter.empty()) {
            if (lineNumber >= numLinesInPort) {
                throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                                      "Requested line number exceeds number of lines in port");
            }
            lineVariables.at(lineNumber) = VariablePtr(lineParameter);
            foundLineParameter = true;
        }
    }
    if (!foundLineParameter) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Digital channel must use at least one line");
    }
}


inline std::string NIDAQDigitalChannel::getLineParameterName(std::size_t lineNumber) {
    return (boost::format("line%d") % lineNumber).str();
}


END_NAMESPACE_MW



























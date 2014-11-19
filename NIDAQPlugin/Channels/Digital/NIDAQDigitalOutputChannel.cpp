//
//  NIDAQDigitalOutputChannel.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 3/14/13.
//  Copyright (c) 2013 MIT. All rights reserved.
//

#include "NIDAQDigitalOutputChannel.h"


BEGIN_NAMESPACE_MW


const std::string NIDAQDigitalOutputChannel::WORD("word");
const std::string NIDAQDigitalOutputChannel::WORD_LINES("word_lines");


void NIDAQDigitalOutputChannel::describeComponent(ComponentInfo &info) {
    NIDAQDigitalChannel::describeComponent(info);
    
    info.setSignature("iochannel/nidaq_digital_output");
    
    info.addParameter(WORD, false);
    info.addParameter(WORD_LINES, false);
}


NIDAQDigitalOutputChannel::NIDAQDigitalOutputChannel(const ParameterValueMap &parameters) :
    NIDAQDigitalChannel(parameters)
{
    if (!(parameters[WORD].empty())) {
        word = VariablePtr(parameters[WORD]);
        
        if (!(parameters[WORD_LINES].empty())) {
            std::vector<Datum> wordLinesValues;
            ParsedExpressionVariable::evaluateExpressionList(parameters[WORD_LINES].str(), wordLinesValues);
            
            for (auto &line : wordLinesValues) {
                int lineNumber = line.getInteger();
                if (lineNumber < 0 || lineNumber >= getNumLinesInPort()) {
                    throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Invalid word line number");
                }
                if (getLineVariable(lineNumber)) {
                    mwarning(M_IODEVICE_MESSAGE_DOMAIN,
                             "Port %d line %d of NIDAQ device will be used for word output; "
                             "attempts to set its value independently will have no effect",
                             getPortNumber(),
                             lineNumber);
                }
                wordLines.push_back(lineNumber);
            }
        }
        
        if (wordLines.empty()) {
            throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Digital output word must use at least one line");
        }
    }
    
    if (!(word || hasLineVariables())) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Digital output channel must use at least one line");
    }
}


void NIDAQDigitalOutputChannel::addNewWordNotification(const boost::shared_ptr<VariableNotification> &vn) const {
    if (word) {
        word->addNotification(vn);
    }
}


void NIDAQDigitalOutputChannel::addNewLineStateNotification(std::size_t lineNumber,
                                                            const boost::shared_ptr<VariableNotification> &vn) const
{
    const VariablePtr &variable = getLineVariable(lineNumber);
    if (variable) {
        variable->addNotification(vn);
    }
}


bool NIDAQDigitalOutputChannel::getLineState(std::size_t lineNumber) const {
    if (word) {
        for (std::size_t i = 0; i < wordLines.size(); i++) {
            if (wordLines[i] == lineNumber) {
                return word->getValue().getInteger() & (1 << i);
            }
        }
    }
    
    const VariablePtr &variable = getLineVariable(lineNumber);
    if (variable) {
        return variable->getValue().getBool();
    }
    
    return false;
}


END_NAMESPACE_MW




























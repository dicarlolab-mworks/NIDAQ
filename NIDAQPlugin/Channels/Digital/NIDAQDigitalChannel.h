//
//  NIDAQDigitalChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 3/13/13.
//  Copyright (c) 2013 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQDigitalChannel__
#define __NIDAQ__NIDAQDigitalChannel__

#include <boost/array.hpp>


BEGIN_NAMESPACE_MW


class NIDAQDigitalChannel : public Component {
    
public:
    static const std::size_t maxNumLines = 32;
    static const std::string PORT_NUMBER;
    static const std::string NUM_LINES_IN_PORT;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQDigitalChannel(const ParameterValueMap &parameters);
    
    int getPortNumber() const { return portNumber; }
    int getNumLinesInPort() const { return numLinesInPort; }
    
protected:
    const VariablePtr& getLineVariable(std::size_t lineNumber) const { return lineVariables.at(lineNumber); }
    
private:
    static std::string getLineParameterName(std::size_t lineNumber);
    
    const int portNumber;
    const int numLinesInPort;
    boost::array<VariablePtr, maxNumLines> lineVariables;
    
};


END_NAMESPACE_MW


#endif // !defined(__NIDAQ__NIDAQDigitalChannel__)

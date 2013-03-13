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
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQDigitalChannel(const ParameterValueMap &parameters);
    
    int getPortNumber() const { return portNumber; }
    
protected:
    typedef boost::array<VariablePtr, maxNumLines> LineVariablesArray;
    const LineVariablesArray& getLineVariables() const { return lineVariables; }
    
private:
    static std::string getLineParameterName(std::size_t lineNumber);
    
    const int portNumber;
    LineVariablesArray lineVariables;
    
};


END_NAMESPACE_MW


#endif // !defined(__NIDAQ__NIDAQDigitalChannel__)

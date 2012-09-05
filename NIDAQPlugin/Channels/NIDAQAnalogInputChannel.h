//
//  NIDAQAnalogInputChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/3/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQAnalogInputChannel__
#define __NIDAQ__NIDAQAnalogInputChannel__

#include "NIDAQAnalogChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQAnalogInputChannel : public NIDAQAnalogChannel {
    
public:
    static const std::string VARIABLE;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQAnalogInputChannel(const ParameterValueMap &parameters);
    
    VariablePtr getVariable() const { return variable; }
    
private:
    VariablePtr variable;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQAnalogInputChannel__) */

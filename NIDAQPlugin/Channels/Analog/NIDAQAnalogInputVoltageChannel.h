//
//  NIDAQAnalogInputVoltageChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/3/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQAnalogInputVoltageChannel__
#define __NIDAQ__NIDAQAnalogInputVoltageChannel__

#include "NIDAQAnalogChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQAnalogInputVoltageChannel : public NIDAQAnalogChannel {
    
public:
    static const std::string VARIABLE;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQAnalogInputVoltageChannel(const ParameterValueMap &parameters);
    
    void postSample(double value, MWTime time) const {
        variable->setValue(value, time);
    }
    
private:
    const VariablePtr variable;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQAnalogInputVoltageChannel__) */

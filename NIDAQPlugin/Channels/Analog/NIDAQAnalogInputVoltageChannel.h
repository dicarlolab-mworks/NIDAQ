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
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQAnalogInputVoltageChannel(const ParameterValueMap &parameters);
    
    void postSample(double value, MWTime time) const {
        voltage->setValue(value, time);
    }
    
private:
    const VariablePtr voltage;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQAnalogInputVoltageChannel__) */

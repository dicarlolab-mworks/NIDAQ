//
//  NIDAQAnalogInputVoltageChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/3/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQAnalogInputVoltageChannel__
#define __NIDAQ__NIDAQAnalogInputVoltageChannel__

#include "NIDAQAnalogInputChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQAnalogInputVoltageChannel : public NIDAQAnalogInputChannel {
    
public:
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQAnalogInputVoltageChannel(const ParameterValueMap &parameters);
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQAnalogInputVoltageChannel__) */

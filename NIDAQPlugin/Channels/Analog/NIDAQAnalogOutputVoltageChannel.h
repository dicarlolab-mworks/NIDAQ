//
//  NIDAQAnalogOutputVoltageChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/12/14.
//  Copyright (c) 2014 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQAnalogOutputVoltageChannel__
#define __NIDAQ__NIDAQAnalogOutputVoltageChannel__

#include "NIDAQAnalogChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQAnalogOutputVoltageChannel : public NIDAQAnalogChannel {
    
public:
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQAnalogOutputVoltageChannel(const ParameterValueMap &parameters);
    
    void addNewVoltageNotification(const boost::shared_ptr<VariableNotification> &vn) const {
        voltage->addNotification(vn);
    }
    
    double getVoltage() const {
        return voltage->getValue().getFloat();
    }
    
private:
    const VariablePtr voltage;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQAnalogOutputVoltageChannel__) */

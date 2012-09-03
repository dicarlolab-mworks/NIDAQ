//
//  NIDAQAnalogInputChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/3/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQAnalogInputChannel__
#define __NIDAQ__NIDAQAnalogInputChannel__


BEGIN_NAMESPACE_MW


class NIDAQAnalogInputChannel : public Component {
    
public:
    static const std::string CHANNEL_NUMBER;
    static const std::string RANGE_MIN;
    static const std::string RANGE_MAX;
    static const std::string VARIABLE;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQAnalogInputChannel(const ParameterValueMap &parameters);
    
    unsigned int getChannelNumber() const { return channelNumber; }
    double getRangeMin() const { return rangeMin; }
    double getRangeMax() const { return rangeMax; }
    VariablePtr getVariable() const { return variable; }
    
private:
    unsigned int channelNumber;
    double rangeMin, rangeMax;
    VariablePtr variable;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQAnalogInputChannel__) */

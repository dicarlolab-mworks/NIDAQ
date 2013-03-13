//
//  NIDAQAnalogChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/5/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQAnalogChannel__
#define __NIDAQ__NIDAQAnalogChannel__


BEGIN_NAMESPACE_MW


class NIDAQAnalogChannel : public Component {
    
public:
    static const std::string CHANNEL_NUMBER;
    static const std::string RANGE_MIN;
    static const std::string RANGE_MAX;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQAnalogChannel(const ParameterValueMap &parameters);
    
    int getChannelNumber() const { return channelNumber; }
    double getRangeMin() const { return rangeMin; }
    double getRangeMax() const { return rangeMax; }
    
private:
    const int channelNumber;
    const double rangeMin;
    const double rangeMax;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQAnalogChannel__) */

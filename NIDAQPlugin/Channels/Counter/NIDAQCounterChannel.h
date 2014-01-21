//
//  NIDAQCounterChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 1/17/14.
//  Copyright (c) 2014 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQCounterChannel__
#define __NIDAQ__NIDAQCounterChannel__


BEGIN_NAMESPACE_MW


class NIDAQCounterChannel : public Component {
    
public:
    static const std::string COUNTER_NUMBER;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQCounterChannel(const ParameterValueMap &parameters);
    
    int getCounterNumber() const { return counterNumber; }
    
private:
    const int counterNumber;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQCounterChannel__) */

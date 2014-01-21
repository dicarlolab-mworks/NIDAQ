//
//  NIDAQCounterInputCountEdgesChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 1/17/14.
//  Copyright (c) 2014 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQCounterInputCountEdgesChannel__
#define __NIDAQ__NIDAQCounterInputCountEdgesChannel__

#include "NIDAQCounterChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQCounterInputCountEdgesChannel : public NIDAQCounterChannel {
    
public:
    static const std::string COUNT;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQCounterInputCountEdgesChannel(const ParameterValueMap &parameters);
    
    void postCount(std::uint32_t value, MWTime time) const;
    
private:
    const VariablePtr count;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQCounterInputCountEdgesChannel__) */

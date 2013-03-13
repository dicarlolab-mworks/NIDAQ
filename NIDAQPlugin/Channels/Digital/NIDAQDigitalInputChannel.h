//
//  NIDAQDigitalInputChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 3/13/13.
//  Copyright (c) 2013 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQDigitalInputChannel__
#define __NIDAQ__NIDAQDigitalInputChannel__

#include <boost/cstdint.hpp>

#include "NIDAQDigitalChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQDigitalInputChannel : public NIDAQDigitalChannel {
    
public:
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQDigitalInputChannel(const ParameterValueMap &parameters);
    
    void postSample(boost::uint32_t value, MWTime time) const;
    
};


END_NAMESPACE_MW


#endif // !defined(__NIDAQ__NIDAQDigitalInputChannel__)

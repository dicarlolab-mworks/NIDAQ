//
//  NIDAQDigitalOutputChannel.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 3/14/13.
//  Copyright (c) 2013 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQDigitalOutputChannel__
#define __NIDAQ__NIDAQDigitalOutputChannel__

#include <boost/cstdint.hpp>

#include "NIDAQDigitalChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQDigitalOutputChannel : public NIDAQDigitalChannel {
    
public:
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQDigitalOutputChannel(const ParameterValueMap &parameters);
    
    void addNewLineStateNotification(std::size_t lineNumber, const boost::shared_ptr<VariableNotification> &vn) const;
    bool getLineState(std::size_t lineNumber) const;
    
};


END_NAMESPACE_MW


#endif // !defined(__NIDAQ__NIDAQDigitalOutputChannel__)

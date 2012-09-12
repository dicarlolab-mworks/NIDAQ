//
//  DigitalIOTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/12/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__DigitalIOTask__
#define __NIDAQ__DigitalIOTask__

#include "Task.h"


BEGIN_NAMESPACE_NIDAQ


class DigitalIOTask : public Task {
    
public:
    DigitalIOTask(const Device &device, const std::string &name) :
        Task(device, name)
    { }
    
    void addChannel(unsigned int portNumber);
    
private:
    virtual int32_t createChannel(const std::string &lines) = 0;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__DigitalIOTask__) */

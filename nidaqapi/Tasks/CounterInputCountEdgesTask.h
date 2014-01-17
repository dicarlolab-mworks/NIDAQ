//
//  CounterInputCountEdgesTask.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 1/17/14.
//  Copyright (c) 2014 MIT. All rights reserved.
//

#ifndef __NIDAQ__CounterInputCountEdgesTask__
#define __NIDAQ__CounterInputCountEdgesTask__

#include "Task.h"


BEGIN_NAMESPACE_NIDAQ


class CounterInputCountEdgesTask : public Task {
    
public:
    std::uint32_t read(double timeout);
    
private:
    CounterInputCountEdgesTask(Device &device, unsigned int counterNumber, bool countRisingEdges);
    
    friend class Device;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__CounterInputCountEdgesTask__) */

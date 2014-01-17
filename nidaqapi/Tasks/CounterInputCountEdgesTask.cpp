//
//  CounterInputCountEdgesTask.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 1/17/14.
//  Copyright (c) 2014 MIT. All rights reserved.
//

#include "CounterInputCountEdgesTask.h"

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


CounterInputCountEdgesTask::CounterInputCountEdgesTask(Device &device,
                                                       unsigned int counterNumber,
                                                       bool countRisingEdges) :
    Task(device)
{
    std::string counter = getChannelName("ctr", counterNumber);
    
    std::int32_t error = DAQmxBaseCreateCICountEdgesChan(getHandle(),
                                                         counter.c_str(),
                                                         NULL,
                                                         (countRisingEdges ? DAQmx_Val_Rising : DAQmx_Val_Falling),
                                                         0,
                                                         DAQmx_Val_CountUp);
    Error::throwIfFailed(error);
    
    Task::addChannel(counter);
}


std::uint32_t CounterInputCountEdgesTask::read(double timeout) {
    nidaqmxbase::uint32_t value;
    Error::throwIfFailed(  DAQmxBaseReadCounterScalarU32(getHandle(), timeout, &value, NULL)  );
    return value;
}


END_NAMESPACE_NIDAQ



























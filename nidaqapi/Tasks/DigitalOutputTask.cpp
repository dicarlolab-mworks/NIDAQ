//
//  DigitalOutputTask.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/12/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "DigitalOutputTask.h"

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


DigitalOutputTask::DigitalOutputTask(Device &device, unsigned int portNumber) :
    Task(device)
{
    std::string lines = getChannelName("port", portNumber);
    
    std::int32_t error = DAQmxBaseCreateDOChan(getHandle(),
                                               lines.c_str(),
                                               NULL,
                                               DAQmx_Val_ChanForAllLines);
    Error::throwIfFailed(error);
    
    Task::addChannel(lines);
}


std::size_t DigitalOutputTask::write(const std::uint32_t &firstSample,
                                     std::size_t numSamples,
                                     double timeout)
{
    nidaqmxbase::int32_t sampsPerChanWritten;
    
    std::int32_t error = DAQmxBaseWriteDigitalU32(getHandle(),
                                                  numSamples,
                                                  FALSE,
                                                  timeout,
                                                  DAQmx_Val_GroupByChannel,
                                                  reinterpret_cast<nidaqmxbase::uint32_t *>(const_cast<std::uint32_t *>(&firstSample)),
                                                  &sampsPerChanWritten,
                                                  NULL);
    Error::throwIfFailed(error);
    
    return std::size_t(sampsPerChanWritten);
}


END_NAMESPACE_NIDAQ



























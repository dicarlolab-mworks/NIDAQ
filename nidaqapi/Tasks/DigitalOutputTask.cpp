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


DigitalOutputTask::DigitalOutputTask(const Device &device) :
    Task(device, "DigitalOutput")
{ }


void DigitalOutputTask::addChannel(unsigned int portNumber) {
    std::string lines = getChannelName("port", portNumber);
    
    int32_t error = DAQmxBaseCreateDOChan(getHandle(),
                                          lines.c_str(),
                                          NULL,
                                          DAQmx_Val_ChanForAllLines);
    Error::throwIfFailed(error);
    
    Task::addChannel(lines);
}


size_t DigitalOutputTask::write(const uint32_t &firstSample,
                                size_t numSamples,
                                double timeout,
                                bool interleaved)
{
    int32_t numSampsPerChan = getNumSamplesPerChannel(numSamples);
    nidaqmxbase::int32_t sampsPerChanWritten;
    
    int32_t error = DAQmxBaseWriteDigitalU32(getHandle(),
                                             numSampsPerChan,
                                             FALSE,
                                             timeout,
                                             (interleaved ? DAQmx_Val_GroupByScanNumber : DAQmx_Val_GroupByChannel),
                                             reinterpret_cast<nidaqmxbase::uint32_t *>(const_cast<uint32_t *>(&firstSample)),
                                             &sampsPerChanWritten,
                                             NULL);
    Error::throwIfFailed(error);
    
    return size_t(sampsPerChanWritten) * getNumChannels();
}


END_NAMESPACE_NIDAQ



























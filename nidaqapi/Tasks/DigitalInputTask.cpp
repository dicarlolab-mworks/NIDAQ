//
//  DigitalInputTask.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/12/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "DigitalInputTask.h"

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


DigitalInputTask::DigitalInputTask(const Device &device) :
    DigitalIOTask(device, "DigitalInputTask")
{ }


int32_t DigitalInputTask::createChannel(const std::string &lines) {
    return DAQmxBaseCreateDIChan(getHandle(),
                                 lines.c_str(),
                                 NULL,
                                 DAQmx_Val_ChanForAllLines);
}


size_t DigitalInputTask::read(uint32_t &firstSample,
                              size_t numSamples,
                              double timeout,
                              bool interleaved)
{
    int32_t numSampsPerChan = getNumSamplesPerChannel(numSamples);
    nidaqmxbase::int32_t sampsPerChanRead = 0;
    
    int32_t error = DAQmxBaseReadDigitalU32(getHandle(),
                                            numSampsPerChan,
                                            timeout,
                                            (interleaved ? DAQmx_Val_GroupByScanNumber : DAQmx_Val_GroupByChannel),
                                            reinterpret_cast<nidaqmxbase::uint32_t *>(&firstSample),
                                            numSamples,
                                            &sampsPerChanRead,
                                            NULL);
    
    // Don't throw if we did a partial read (not sure this ever happens, but just to be safe ...)
    if ((error != DAQmxErrorSamplesNotYetAvailable) || (sampsPerChanRead == 0)) {
        Error::throwIfFailed(error);
    }
    
    return size_t(sampsPerChanRead) * getNumChannels();
}


END_NAMESPACE_NIDAQ



























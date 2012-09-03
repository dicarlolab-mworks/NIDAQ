//
//  AnalogOutputTask.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "AnalogOutputTask.h"

#include <boost/format.hpp>

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


AnalogOutputTask::AnalogOutputTask(const Device &device) :
    Task(device, "AnalogOutputTask")
{ }


void AnalogOutputTask::addVoltageChannel(unsigned int channelNumber,
                                         double minVal,
                                         double maxVal)
{
    std::string physicalChannel = (boost::format("%s/ao%u") % getDeviceName() % channelNumber).str();
    
    int32_t error = DAQmxBaseCreateAOVoltageChan(getHandle(),
                                                 physicalChannel.c_str(),
                                                 NULL,
                                                 minVal,
                                                 maxVal,
                                                 DAQmx_Val_Volts,
                                                 NULL);
    Error::throwIfFailed(error);
    
    addChannel();
}


int32_t AnalogOutputTask::write(const double &firstSample,
                                std::size_t numSamples,
                                double timeout,
                                bool interleaved)
{
    int32_t numSampsPerChan = getNumSamplesPerChannel(numSamples);
    nidaqmxbase::int32_t sampsPerChanWritten;
    
    int32_t error = DAQmxBaseWriteAnalogF64(getHandle(),
                                            numSampsPerChan,
                                            FALSE,
                                            timeout,
                                            (interleaved ? DAQmx_Val_GroupByScanNumber : DAQmx_Val_GroupByChannel),
                                            const_cast<double *>(&firstSample),
                                            &sampsPerChanWritten,
                                            NULL);
    Error::throwIfFailed(error);
    
    return sampsPerChanWritten;
}


END_NAMESPACE_NIDAQ




























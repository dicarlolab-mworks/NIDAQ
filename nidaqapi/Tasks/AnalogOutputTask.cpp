//
//  AnalogOutputTask.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "AnalogOutputTask.h"

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


Device::AnalogOutputTask::AnalogOutputTask(const Device &device) :
    Task(device, "analog output")
{ }


void Device::AnalogOutputTask::setAllowRegeneration(bool allowRegen) {
    std::int32_t value = (allowRegen ? DAQmx_Val_AllowRegen : DAQmx_Val_DoNotAllowRegen);
    Error::throwIfFailed(  DAQmxBaseSetWriteAttribute(getHandle(), DAQmx_Write_RegenMode, value)  );
}


void Device::AnalogOutputTask::addVoltageChannel(unsigned int channelNumber,
                                         double minVal,
                                         double maxVal)
{
    std::string physicalChannel = getChannelName("ao", channelNumber);
    
    std::int32_t error = DAQmxBaseCreateAOVoltageChan(getHandle(),
                                                 physicalChannel.c_str(),
                                                 NULL,
                                                 minVal,
                                                 maxVal,
                                                 DAQmx_Val_Volts,
                                                 NULL);
    Error::throwIfFailed(error);
    
    Task::addChannel(physicalChannel);
}


std::size_t Device::AnalogOutputTask::write(const double &firstSample,
                               std::size_t numSamples,
                               double timeout,
                               bool interleaved)
{
    std::int32_t numSampsPerChan = getNumSamplesPerChannel(numSamples);
    nidaqmxbase::int32_t sampsPerChanWritten;
    
    std::int32_t error = DAQmxBaseWriteAnalogF64(getHandle(),
                                            numSampsPerChan,
                                            FALSE,
                                            timeout,
                                            (interleaved ? DAQmx_Val_GroupByScanNumber : DAQmx_Val_GroupByChannel),
                                            const_cast<double *>(&firstSample),
                                            &sampsPerChanWritten,
                                            NULL);
    Error::throwIfFailed(error);
    
    return std::size_t(sampsPerChanWritten) * getNumChannels();
}


END_NAMESPACE_NIDAQ




























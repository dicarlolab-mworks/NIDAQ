//
//  NIDAQAnalogOutputTask.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQAnalogOutputTask.h"

#include <boost/format.hpp>

#include "NIDAQmxBaseAPI.h"
#include "NIDAQError.h"


NIDAQAnalogOutputTask::NIDAQAnalogOutputTask(const NIDAQDevice &device) :
    NIDAQTask(device, "AnalogOutputTask")
{ }


void NIDAQAnalogOutputTask::addVoltageChannel(unsigned int channelNumber,
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
    NIDAQError::throwIfFailed(error);
    
    addChannel(physicalChannel);
}


int32_t NIDAQAnalogOutputTask::write(const std::vector<double> &samples,
                                     double timeout,
                                     bool interleaved)
{
    int32_t numSampsPerChan = getNumSamplesPerChannel(samples);
    nidaqmxbase::int32_t sampsPerChanWritten;
    
    int32_t error = DAQmxBaseWriteAnalogF64(getHandle(),
                                            numSampsPerChan,
                                            FALSE,
                                            timeout,
                                            (interleaved ? DAQmx_Val_GroupByScanNumber : DAQmx_Val_GroupByChannel),
                                            &(const_cast< std::vector<double>& >(samples).front()),
                                            &sampsPerChanWritten,
                                            NULL);
    NIDAQError::throwIfFailed(error);
    
    return sampsPerChanWritten;
}




























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
    NIDAQError::throwOnFailure(error);
    
    addChannel();
}


int32_t NIDAQAnalogOutputTask::write(double timeout,
                                     const std::vector<double> &samples,
                                     bool interleaved)
{
    if ((getNumChannels() == 0) || (samples.size() % getNumChannels() != 0))
    {
        throw std::invalid_argument("Invalid number of samples");
    }
    
    int32_t numSampsPerChan = samples.size() / getNumChannels();
    int32 sampsPerChanWritten;
    
    int32_t error = DAQmxBaseWriteAnalogF64(getHandle(),
                                            numSampsPerChan,
                                            FALSE,
                                            timeout,
                                            (interleaved ? DAQmx_Val_GroupByScanNumber : DAQmx_Val_GroupByChannel),
                                            &(const_cast< std::vector<double>& >(samples).front()),
                                            &sampsPerChanWritten,
                                            NULL);
    NIDAQError::throwOnFailure(error);
    
    return sampsPerChanWritten;
}




























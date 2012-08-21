//
//  NIDAQAnalogOutputTask.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQAnalogOutputTask.h"

#include "NIDAQmxBaseAPI.h"
#include "NIDAQError.h"


void NIDAQAnalogOutputTask::createVoltageChannel(const std::string &physicalChannel,
                                                 double minVal,
                                                 double maxVal)
{
    int error = DAQmxBaseCreateAOVoltageChan(TaskHandle(getHandle()),
                                             physicalChannel.c_str(),
                                             NULL,
                                             minVal,
                                             maxVal,
                                             DAQmx_Val_Volts,
                                             NULL);
    NIDAQError::throwOnFailure(error);
}


int NIDAQAnalogOutputTask::write(int numSampsPerChan,
                                 double timeout,
                                 bool interleaved,
                                 const std::vector<double> &writeArray)
{
    int32 sampsPerChanWritten;
    
    int error = DAQmxBaseWriteAnalogF64(TaskHandle(getHandle()),
                                        numSampsPerChan,
                                        FALSE,
                                        timeout,
                                        (interleaved ? DAQmx_Val_GroupByScanNumber : DAQmx_Val_GroupByChannel),
                                        &(const_cast< std::vector<double>& >(writeArray).front()),
                                        &sampsPerChanWritten,
                                        NULL);
    NIDAQError::throwOnFailure(error);
    
    return sampsPerChanWritten;
}

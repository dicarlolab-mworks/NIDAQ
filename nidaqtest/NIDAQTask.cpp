//
//  NIDAQTask.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQTask.h"

#include "NIDAQDevice.h"
#include "NIDAQError.h"


NIDAQTask::~NIDAQTask() {
    if (running) {
        NIDAQError::logOnFailure(  DAQmxBaseStopTask(handle)  );
    }
    NIDAQError::logOnFailure(  DAQmxBaseClearTask(handle)  );
}


NIDAQTask::NIDAQTask(const NIDAQDevice &device, const std::string &name) :
    deviceName(device.getName()),
    running(false)
{
    NIDAQError::throwOnFailure(  DAQmxBaseCreateTask(name.c_str(), &handle)  );
}


void NIDAQTask::createAnalogOutputVoltageChannel(const std::string &physicalChannel, float64 minVal, float64 maxVal) {
    int32 error = DAQmxBaseCreateAOVoltageChan(handle,
                                               physicalChannel.c_str(),
                                               NULL,
                                               minVal,
                                               maxVal,
                                               DAQmx_Val_Volts,
                                               NULL);
    NIDAQError::throwOnFailure(error);
}


void NIDAQTask::configureSampleClockTiming(const std::string &source,
                                           float64 rate,
                                           int32 activeEdge,
                                           int32 sampleMode,
                                           uInt64 sampsPerChanToAcquire)
{
    int32 error = DAQmxBaseCfgSampClkTiming(handle,
                                            source.c_str(),
                                            rate,
                                            activeEdge,
                                            sampleMode,
                                            sampsPerChanToAcquire);
    NIDAQError::throwOnFailure(error);
}


int32 NIDAQTask::writeAnalog(int32 numSampsPerChan,
                             float64 timeout,
                             bool32 dataLayout,
                             const std::vector<float64> &writeArray)
{
    int32 sampsPerChanWritten;
    
    int32 error = DAQmxBaseWriteAnalogF64(handle,
                                          numSampsPerChan,
                                          FALSE,
                                          timeout,
                                          dataLayout,
                                          &(const_cast< std::vector<float64>& >(writeArray).front()),
                                          &sampsPerChanWritten,
                                          NULL);
    NIDAQError::throwOnFailure(error);
    
    return sampsPerChanWritten;
}


void NIDAQTask::start() {
    if (!running) {
        NIDAQError::throwOnFailure(  DAQmxBaseStartTask(handle)  );
        running = true;
    }
}


void NIDAQTask::stop() {
    if (running) {
        NIDAQError::throwOnFailure(  DAQmxBaseStopTask(handle)  );
        running = false;
    }
}



























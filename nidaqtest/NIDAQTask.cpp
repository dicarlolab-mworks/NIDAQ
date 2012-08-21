//
//  NIDAQTask.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQTask.h"

#include "NIDAQmxBaseAPI.h"
#include "NIDAQDevice.h"
#include "NIDAQError.h"


NIDAQTask::~NIDAQTask() {
    if (running) {
        NIDAQError::logOnFailure(  DAQmxBaseStopTask(TaskHandle(handle))  );
    }
    NIDAQError::logOnFailure(  DAQmxBaseClearTask(TaskHandle(handle))  );
}


NIDAQTask::NIDAQTask(const NIDAQDevice &device, const std::string &name) :
    deviceName(device.getName()),
    running(false)
{
    TaskHandle h;
    NIDAQError::throwOnFailure(  DAQmxBaseCreateTask(name.c_str(), &h)  );
    handle = h;
}


void NIDAQTask::createAnalogOutputVoltageChannel(const std::string &physicalChannel, double minVal, double maxVal) {
    int error = DAQmxBaseCreateAOVoltageChan(TaskHandle(handle),
                                             physicalChannel.c_str(),
                                             NULL,
                                             minVal,
                                             maxVal,
                                             DAQmx_Val_Volts,
                                             NULL);
    NIDAQError::throwOnFailure(error);
}


void NIDAQTask::configureSampleClockTiming(const std::string &source,
                                           double rate,
                                           bool acquireOnRisingEdge,
                                           bool continous,
                                           unsigned long long sampsPerChanToAcquire)
{
    int error = DAQmxBaseCfgSampClkTiming(TaskHandle(handle),
                                          source.c_str(),
                                          rate,
                                          (acquireOnRisingEdge ? DAQmx_Val_Rising : DAQmx_Val_Falling),
                                          (continous ? DAQmx_Val_ContSamps : DAQmx_Val_FiniteSamps),
                                          sampsPerChanToAcquire);
    NIDAQError::throwOnFailure(error);
}


int NIDAQTask::writeAnalog(int numSampsPerChan,
                           double timeout,
                           bool interleaved,
                           const std::vector<double> &writeArray)
{
    int32 sampsPerChanWritten;
    
    int error = DAQmxBaseWriteAnalogF64(TaskHandle(handle),
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


void NIDAQTask::start() {
    if (!running) {
        NIDAQError::throwOnFailure(  DAQmxBaseStartTask(TaskHandle(handle))  );
        running = true;
    }
}


void NIDAQTask::stop() {
    if (running) {
        NIDAQError::throwOnFailure(  DAQmxBaseStopTask(TaskHandle(handle))  );
        running = false;
    }
}



























//
//  NIDAQTask.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQTask.h"

#include "NIDAQmxBaseAPI.h"
#include "NIDAQError.h"


NIDAQTask::~NIDAQTask() {
    if (running) {
        NIDAQError::logOnFailure(  DAQmxBaseStopTask(TaskHandle(getHandle()))  );
    }
    NIDAQError::logOnFailure(  DAQmxBaseClearTask(TaskHandle(getHandle()))  );
}


NIDAQTask::NIDAQTask(const NIDAQDevice &device, const std::string &name) :
    deviceName(device.getName()),
    running(false)
{
    TaskHandle h;
    NIDAQError::throwOnFailure(  DAQmxBaseCreateTask(name.c_str(), &h)  );
    handle = h;
}


void NIDAQTask::configureSampleClockTiming(const std::string &source,
                                           double rate,
                                           bool acquireOnRisingEdge,
                                           bool continous,
                                           unsigned long long sampsPerChanToAcquire)
{
    int error = DAQmxBaseCfgSampClkTiming(TaskHandle(getHandle()),
                                          source.c_str(),
                                          rate,
                                          (acquireOnRisingEdge ? DAQmx_Val_Rising : DAQmx_Val_Falling),
                                          (continous ? DAQmx_Val_ContSamps : DAQmx_Val_FiniteSamps),
                                          sampsPerChanToAcquire);
    NIDAQError::throwOnFailure(error);
}


void NIDAQTask::start() {
    if (!running) {
        NIDAQError::throwOnFailure(  DAQmxBaseStartTask(TaskHandle(getHandle()))  );
        running = true;
    }
}


void NIDAQTask::stop() {
    if (running) {
        NIDAQError::throwOnFailure(  DAQmxBaseStopTask(TaskHandle(getHandle()))  );
        running = false;
    }
}



























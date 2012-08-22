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


NIDAQTask::NIDAQTask(const NIDAQDevice &device, const std::string &name) :
    deviceName(device.getName()),
    numChannels(0),
    running(false)
{
    std::string taskName = deviceName + "/" + name;
    NIDAQError::throwOnFailure(  DAQmxBaseCreateTask(taskName.c_str(), &handle)  );
}


NIDAQTask::~NIDAQTask() {
    if (running) {
        NIDAQError::logOnFailure(  DAQmxBaseStopTask(getHandle())  );
    }
    NIDAQError::logOnFailure(  DAQmxBaseClearTask(getHandle())  );
}


void NIDAQTask::setSampleClockTiming(double samplingRate,
                                     const std::string &clockSourceTerminal,
                                     uint64_t samplesPerChannelToAcquire,
                                     bool acquireOnRisingEdge)
{
    bool continous = (samplesPerChannelToAcquire == 0);
    int32_t error = DAQmxBaseCfgSampClkTiming(getHandle(),
                                              clockSourceTerminal.c_str(),
                                              samplingRate,
                                              (acquireOnRisingEdge ? DAQmx_Val_Rising : DAQmx_Val_Falling),
                                              (continous ? DAQmx_Val_ContSamps : DAQmx_Val_FiniteSamps),
                                              samplesPerChannelToAcquire);
    NIDAQError::throwOnFailure(error);
}


void NIDAQTask::start() {
    if (!running) {
        NIDAQError::throwOnFailure(  DAQmxBaseStartTask(getHandle())  );
        running = true;
    }
}


void NIDAQTask::stop() {
    if (running) {
        NIDAQError::throwOnFailure(  DAQmxBaseStopTask(getHandle())  );
        running = false;
    }
}



























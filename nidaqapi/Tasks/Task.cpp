//
//  Task.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "Task.h"

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


Task::Task(const Device &device, const std::string &name) :
    deviceName(device.getName()),
    numChannels(0),
    running(false)
{
    std::string taskName = deviceName + "/" + name;
    Error::throwIfFailed(  DAQmxBaseCreateTask(taskName.c_str(), &handle)  );
}


Task::~Task() {
    if (running) {
        Error::logIfFailed(  DAQmxBaseStopTask(getHandle())  );
    }
    Error::logIfFailed(  DAQmxBaseClearTask(getHandle())  );
}


void Task::setSampleClockTiming(double samplingRate,
                                uint64_t samplesPerChannelToAcquire,
                                bool continous,
                                const std::string &clockSourceTerminal,
                                bool acquireOnRisingEdge)
{
    int32_t error = DAQmxBaseCfgSampClkTiming(getHandle(),
                                              clockSourceTerminal.c_str(),
                                              samplingRate,
                                              (acquireOnRisingEdge ? DAQmx_Val_Rising : DAQmx_Val_Falling),
                                              (continous ? DAQmx_Val_ContSamps : DAQmx_Val_FiniteSamps),
                                              samplesPerChannelToAcquire);
    Error::throwIfFailed(error);
}


void Task::start() {
    if (!running) {
        Error::throwIfFailed(  DAQmxBaseStartTask(getHandle())  );
        running = true;
    }
}


void Task::stop() {
    if (running) {
        Error::throwIfFailed(  DAQmxBaseStopTask(getHandle())  );
        running = false;
    }
}


END_NAMESPACE_NIDAQ



























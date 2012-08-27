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
    Error::throwIfFailed(error);
}


void Task::setAllowRegeneration(bool allowRegen) {
    int32_t value = (allowRegen ? DAQmx_Val_AllowRegen : DAQmx_Val_DoNotAllowRegen);
    Error::throwIfFailed(  DAQmxBaseSetWriteAttribute(getHandle(), DAQmx_Write_RegenMode, value)  );
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



























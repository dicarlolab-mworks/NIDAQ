//
//  Task.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "Task.h"

#include <boost/format.hpp>

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


std::set<std::string> Task::allTaskNames;


Task::Task(const Device &device, const std::string &taskType) :
    deviceName(device.getName()),
    taskName(deviceName + "/" + taskType),
    numChannels(0),
    running(false)
{
    if (!(allTaskNames.insert(taskName).second)) {
        throw std::logic_error("Task " + taskName + " already exists");
    }
    Error::throwIfFailed(  DAQmxBaseCreateTask(taskName.c_str(), &handle)  );
}


Task::~Task() {
    if (running) {
        Error::logIfFailed(  DAQmxBaseStopTask(getHandle())  );
    }
    Error::logIfFailed(  DAQmxBaseClearTask(getHandle())  );
    allTaskNames.erase(taskName);
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


std::string Task::getChannelName(const std::string &type, unsigned int number) const {
    return (boost::format("%s/%s%u") % getDeviceName() % type % number).str();
}


END_NAMESPACE_NIDAQ



























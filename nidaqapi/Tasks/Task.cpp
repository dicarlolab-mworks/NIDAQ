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
#include "Device.h"


BEGIN_NAMESPACE_NIDAQ


Task::Task(Device &device) :
    device(device),
    handle(NULL),
    numChannels(0),
    running(false)
{
    Error::throwIfFailed(  DAQmxBaseCreateTask("", &handle)  );
}


Task::~Task() {
    if (running) {
        Error::logIfFailed(  DAQmxBaseStopTask(getHandle())  );
    }
    Error::logIfFailed(  DAQmxBaseClearTask(getHandle())  );
}


void Task::setSampleClockTiming(double samplingRate,
                                std::uint64_t samplesPerChannelToAcquire,
                                bool continous,
                                const std::string &clockSourceTerminal,
                                bool acquireOnRisingEdge)
{
    std::int32_t error = DAQmxBaseCfgSampClkTiming(getHandle(),
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
    return (boost::format("%s/%s%u") % device.getName() % type % number).str();
}


void Task::addChannel(const std::string &name) {
    if (!(device.channelNames.insert(name).second)) {
        throw Error("Channel " + name + " is already in use");
    }
    numChannels++;
}


std::int32_t Task::getNumSamplesPerChannel(std::size_t numSamples) const {
    if ((numChannels == 0) || (numSamples % numChannels != 0))
    {
        throw std::invalid_argument("Invalid number of samples");
    }
    return numSamples / numChannels;
}


END_NAMESPACE_NIDAQ



























//
//  Device.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "Device.h"

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


Device::Device(const std::string &name) :
    name(name),
    serialNumber(getSerialNumber(name))
{ }


Device::~Device() {
    // Destroy all tasks before resetting the device
    analogInputTask.reset();
    analogOutputTask.reset();
    digitalInputTask.reset();
    digitalOutputTasks.clear();
    counterInputCountEdgesTasks.clear();
    
    Error::logIfFailed(  DAQmxBaseResetDevice(name.c_str())  );
}


AnalogInputTask& Device::getAnalogInputTask() {
    if (!analogInputTask) {
        analogInputTask.reset(new AnalogInputTask(*this));
    }
    return *analogInputTask;
}


AnalogOutputTask& Device::getAnalogOutputTask() {
    if (!analogOutputTask) {
        analogOutputTask.reset(new AnalogOutputTask(*this));
    }
    return *analogOutputTask;
}


DigitalInputTask& Device::getDigitalInputTask() {
    if (!digitalInputTask) {
        digitalInputTask.reset(new DigitalInputTask(*this));
    }
    return *digitalInputTask;
}


DigitalOutputTask& Device::getDigitalOutputTask(unsigned int portNumber) {
    std::unique_ptr<DigitalOutputTask> &task = digitalOutputTasks[portNumber];
    if (!task) {
        task.reset(new DigitalOutputTask(*this, portNumber));
    }
    return *task;
}


CounterInputCountEdgesTask& Device::getCounterInputCountEdgesTask(unsigned int counterNumber) {
    std::unique_ptr<CounterInputCountEdgesTask> &task = counterInputCountEdgesTasks[counterNumber];
    if (!task) {
        task.reset(new CounterInputCountEdgesTask(*this, counterNumber));
    }
    return *task;
}


void Device::clearAnalogInputTask() {
    if (analogInputTask) {
        analogInputTask->stop();
        analogInputTask.reset();
    }
}


void Device::clearAnalogOutputTask() {
    if (analogOutputTask) {
        analogOutputTask->stop();
        analogOutputTask.reset();
    }
}


void Device::clearDigitalInputTask() {
    if (digitalInputTask) {
        digitalInputTask->stop();
        digitalInputTask.reset();
    }
}


void Device::clearDigitalOutputTask(unsigned int portNumber) {
    std::unique_ptr<DigitalOutputTask> &task = digitalOutputTasks[portNumber];
    if (task) {
        task->stop();
        task.reset();
    }
}


void Device::clearCounterInputCountEdgesTask(unsigned int counterNumber) {
    std::unique_ptr<CounterInputCountEdgesTask> &task = counterInputCountEdgesTasks[counterNumber];
    if (task) {
        task->stop();
        task.reset();
    }
}


std::uint32_t Device::getSerialNumber(const std::string &deviceName) {
    nidaqmxbase::uint32_t serialNumber = 0;
    Error::throwIfFailed(  DAQmxBaseGetDevSerialNum(deviceName.c_str(), &serialNumber)  );
    return serialNumber;
}


END_NAMESPACE_NIDAQ




























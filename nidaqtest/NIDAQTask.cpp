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


NIDAQTask::NIDAQTask(const std::string &name, const NIDAQDevice &device) :
    deviceName(device.getName()),
    running(false)
{
    NIDAQError::throwOnFailure(  DAQmxBaseCreateTask(name.c_str(), &handle)  );
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

//
//  NIDAQPluginHelper.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQPluginHelper__
#define __NIDAQ__NIDAQPluginHelper__

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "Device.h"
#include "AnalogInputTask.h"
#include "AnalogOutputTask.h"

#include "MachClock.h"

#include "IPCRequestResponse.h"
#include "HelperControlMessage.h"

#include "NIDAQPluginHelperError.h"


class NIDAQPluginHelper : boost::noncopyable {
    
public:
    NIDAQPluginHelper(IPCRequestResponse &ipc,
                      HelperControlMessage &message,
                      const std::string &deviceName);
    
    bool handleRequests();
    
private:
    void handleRequest(bool &done);
    
    void requireAnalogInputTask();
    void createAnalogInputVoltageChannel();
    void setAnalogInputSampleClockTiming();
    void startAnalogInputTask();
    void stopAnalogInputTask();
    void readAnalogInputSamples();
    
    void requireAnalogOutputTask();
    void createAnalogOutputVoltageChannel();
    void setAnalogOutputSampleClockTiming();
    void startAnalogOutputTask();
    void stopAnalogOutputTask();
    void writeAnalogOutputSamples();
    
    IPCRequestResponse &ipc;
    HelperControlMessage &m;
    
    nidaq::Device device;
    boost::scoped_ptr<nidaq::AnalogInputTask> analogInputTask;
    boost::scoped_ptr<nidaq::AnalogOutputTask> analogOutputTask;
    
    MachClock clock;
    
};


#endif /* !defined(__NIDAQ__NIDAQPluginHelper__) */





























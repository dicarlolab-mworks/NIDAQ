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
#include "DigitalInputTask.h"
#include "DigitalOutputTask.h"

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
    void readAnalogInputSamples();
    void clearAnalogInputTask();
    
    void requireAnalogOutputTask();
    void createAnalogOutputVoltageChannel();
    void setAnalogOutputSampleClockTiming();
    void startAnalogOutputTask();
    void writeAnalogOutputSamples();
    void clearAnalogOutputTask();
    
    void requireDigitalInputTask();
    void createDigitalInputChannel();
    void startDigitalInputTask();
    void readDigitalInputSamples();
    void clearDigitalInputTask();
    
    void requireDigitalOutputTask();
    void createDigitalOutputChannel();
    void startDigitalOutputTask();
    void writeDigitalOutputSamples();
    void clearDigitalOutputTask();
    
    IPCRequestResponse &ipc;
    HelperControlMessage &m;
    
    nidaq::Device device;
    boost::scoped_ptr<nidaq::AnalogInputTask> analogInputTask;
    boost::scoped_ptr<nidaq::AnalogOutputTask> analogOutputTask;
    boost::scoped_ptr<nidaq::DigitalInputTask> digitalInputTask;
    boost::scoped_ptr<nidaq::DigitalOutputTask> digitalOutputTask;
    
    MachClock clock;
    
};


#endif /* !defined(__NIDAQ__NIDAQPluginHelper__) */





























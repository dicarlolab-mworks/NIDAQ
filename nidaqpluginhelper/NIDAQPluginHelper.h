//
//  NIDAQPluginHelper.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQPluginHelper__
#define __NIDAQ__NIDAQPluginHelper__

#include <memory>
#include <set>

#include <boost/noncopyable.hpp>

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
    
    nidaq::Device& getDevice();
    
    void createAnalogInputVoltageChannel();
    void setAnalogInputSampleClockTiming();
    void startAnalogInputTask();
    void readAnalogInputSamples();
    void clearAnalogInputTask();
    
    void createAnalogOutputVoltageChannel();
    void setAnalogOutputSampleClockTiming();
    void startAnalogOutputTask();
    void writeAnalogOutputSamples();
    void clearAnalogOutputTask();
    
    void createDigitalInputChannel();
    void startDigitalInputTask();
    void readDigitalInputSamples();
    void clearDigitalInputTask();
    
    void createDigitalOutputChannel();
    void startDigitalOutputTasks();
    void writeDigitalOutputSamples();
    void clearDigitalOutputTasks();
    
    IPCRequestResponse &ipc;
    HelperControlMessage &m;
    const std::string deviceName;
    
    std::unique_ptr<nidaq::Device> device;
    std::set<unsigned int> digitalOutputPortNumbers;
    
    MachClock clock;
    
};


#endif /* !defined(__NIDAQ__NIDAQPluginHelper__) */





























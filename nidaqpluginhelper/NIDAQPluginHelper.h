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
    
    IPCRequestResponse &ipc;
    HelperControlMessage &m;
    
    nidaq::Device device;
    boost::scoped_ptr<nidaq::AnalogInputTask> analogInputTask;
    
};


#endif /* !defined(__NIDAQ__NIDAQPluginHelper__) */





























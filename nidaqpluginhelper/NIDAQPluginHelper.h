//
//  NIDAQPluginHelper.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQPluginHelper__
#define __NIDAQ__NIDAQPluginHelper__

#include <boost/scoped_ptr.hpp>

#include "Device.h"
#include "AnalogInputTask.h"

#include "IPCRequestResponse.h"
#include "HelperControlMessage.h"


class NIDAQPluginHelper {
    
public:
    NIDAQPluginHelper(const std::string &deviceName,
                      const std::string &wantRequestName,
                      const std::string &wantResponseName,
                      HelperControlMessage &message);
    
    bool handleControlRequests();
    
private:
    void handleControlRequest(bool &done);
    
    void createAnalogInputTask();
    void createAnalogInputVoltageChannel(unsigned int channelNumber, double minVal, double maxVal);
    void setAnalogInputSampleClockTiming(double samplingRate);
    
    nidaq::Device device;
    IPCRequestResponse ipc;
    HelperControlMessage &m;
    
    boost::scoped_ptr<nidaq::AnalogInputTask> analogInputTask;
    
};


#endif /* !defined(__NIDAQ__NIDAQPluginHelper__) */

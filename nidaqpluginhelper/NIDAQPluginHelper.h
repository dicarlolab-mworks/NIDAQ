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

#include "NIDAQPluginHelperError.h"


class NIDAQPluginHelper {
    
public:
    NIDAQPluginHelper(const std::string &deviceName,
                      const std::string &wantRequestName,
                      const std::string &wantResponseName,
                      HelperControlMessage &message);
    
    bool handleControlRequests();
    
private:
    void handleControlRequest(bool &done);
    
    void validateAnalogInputTask() {
        if (!analogInputTask) {
            throw NIDAQPluginHelperError("Analog input task has not been created");
        }
    }
    void createAnalogInputTask();
    void createAnalogInputVoltageChannel(unsigned int channelNumber, double minVal, double maxVal);
    void setAnalogInputSampleClockTiming(double samplingRate);
    void startAnalogInputTask();
    void stopAnalogInputTask();
    void readAnalogInputSamples();
    
    nidaq::Device device;
    IPCRequestResponse ipc;
    HelperControlMessage &m;
    
    boost::scoped_ptr<nidaq::AnalogInputTask> analogInputTask;
    
};


#endif /* !defined(__NIDAQ__NIDAQPluginHelper__) */





























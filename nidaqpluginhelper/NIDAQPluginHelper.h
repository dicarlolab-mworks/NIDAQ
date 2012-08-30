//
//  NIDAQPluginHelper.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQPluginHelper__
#define __NIDAQ__NIDAQPluginHelper__

#include "Device.h"

#include "HelperControlMessage.h"


class NIDAQPluginHelper {
    
public:
    NIDAQPluginHelper(const std::string &deviceName, HelperControlChannel &controlChannel);
    
    bool handleControlRequests();
    
private:
    void handleControlRequest(HelperControlMessage &m, bool &done);
    
    nidaq::Device device;
    HelperControlChannel &controlChannel;
    
};


#endif /* !defined(__NIDAQ__NIDAQPluginHelper__) */

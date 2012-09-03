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
    
    nidaq::Device device;
    IPCRequestResponse ipc;
    HelperControlMessage &m;
    
};


#endif /* !defined(__NIDAQ__NIDAQPluginHelper__) */

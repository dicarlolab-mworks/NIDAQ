//
//  NIDAQDevice.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQDevice.h"

#include <spawn.h>
#include <sys/wait.h>


BEGIN_NAMESPACE_MW


const std::string NIDAQDevice::NAME("name");


void NIDAQDevice::describeComponent(ComponentInfo &info) {
    IODevice::describeComponent(info);
    
    info.setSignature("iodevice/nidaq");
    
    info.addParameter(NAME, true, "Dev1");
}


NIDAQDevice::NIDAQDevice(const ParameterValueMap &parameters) :
    IODevice(parameters)
{
    pid_t pid;
    const char * const argv[2] = { PLUGIN_HELPER_EXECUTABLE, 0 };
    int status = posix_spawn(&pid,
                             PLUGIN_HELPERS_DIR "/" PLUGIN_HELPER_EXECUTABLE,
                             NULL,
                             NULL,
                             const_cast<char * const *>(argv),
                             NULL);
    
    if (0 == status) {
        mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Successfully launched helper process");
        int stat;
        waitpid(pid, &stat, 0);
    } else {
        merror(M_IODEVICE_MESSAGE_DOMAIN, "Helper process failed to launch (status = %d)", status);
    }
}


END_NAMESPACE_MW




























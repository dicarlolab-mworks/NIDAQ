//
//  main.cpp
//  nidaqpluginhelper
//
//  Created by Christopher Stawarz on 8/24/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <cstdlib>
#include <iostream>

#include "HelperSharedMemory.h"
#include "NIDAQPluginHelper.h"


int main(int argc, const char * argv[])
{
    if (argc != 5) {
        std::cerr << argv[0] << " requires 4 arguments" << std::endl;
        return EXIT_FAILURE;
    }
    
    const char *requestSemName = argv[1];
    const char *responseSemName = argv[2];
    const char *sharedMemoryName = argv[3];
    const char *deviceName = argv[4];
    
    IPCRequestResponse ipc(boost::interprocess::open_only, requestSemName, responseSemName);
    HelperSharedMemory sharedMemory(boost::interprocess::open_only, sharedMemoryName);
    
    NIDAQPluginHelper helper(ipc, *(sharedMemory.getMessagePtr()), deviceName);
    bool success = helper.handleRequests();
    
    return (success ? EXIT_SUCCESS : EXIT_FAILURE);
}




























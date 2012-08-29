//
//  main.cpp
//  nidaqpluginhelper
//
//  Created by Christopher Stawarz on 8/24/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>

#include "Device.h"

#include "IPCRequestResponse.h"
#include "HelperControlMessage.h"


int main(int argc, const char * argv[])
{
    mw::StandardServerCoreBuilder coreBuilder;
    mw::CoreBuilderForeman::constructCoreStandardOrder(&coreBuilder);
    
    IPCRequestResponse<int> ipc;
    
    HelperControlMessage msg;
    msg.createDeviceRequest.name = "Dev1";
    
    const char *deviceName = argv[1];
    nidaq::Device device(deviceName);
    std::cout << std::endl << "device name = \"" << device.getName() << "\"" << std::endl;
    
    const char *sharedMemoryName = argv[2];
    std::cout << "sharedMemoryName = \"" << sharedMemoryName << "\"" << std::endl;
    
    boost::interprocess::shared_memory_object sharedMemory(boost::interprocess::open_only,
                                                           sharedMemoryName,
                                                           boost::interprocess::read_write);
    std::cout << "Successfully opened shared memory" << std::endl;
    
    return 0;
}




























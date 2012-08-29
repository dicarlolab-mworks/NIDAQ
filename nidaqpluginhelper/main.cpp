//
//  main.cpp
//  nidaqpluginhelper
//
//  Created by Christopher Stawarz on 8/24/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <iostream>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "Device.h"

#include "HelperControlMessage.h"


int main(int argc, const char * argv[])
{
    mw::StandardServerCoreBuilder coreBuilder;
    mw::CoreBuilderForeman::constructCoreStandardOrder(&coreBuilder);
    
    const char *deviceName = argv[1];
    nidaq::Device device(deviceName);
    std::cout << std::endl << "device name = \"" << device.getName() << "\"" << std::endl;
    
    const char *sharedMemoryName = argv[2];
    std::cout << "sharedMemoryName = \"" << sharedMemoryName << "\"" << std::endl;
    
    boost::interprocess::shared_memory_object sharedMemory(boost::interprocess::open_only,
                                                           sharedMemoryName,
                                                           boost::interprocess::read_write);
    std::cout << "Successfully opened shared memory" << std::endl;
    
    boost::interprocess::mapped_region mappedRegion(sharedMemory, boost::interprocess::read_write);
    void *address = mappedRegion.get_address();
    HelperControlChannel *controlChannel = static_cast<HelperControlChannel *>(address);
    
    boost::posix_time::time_duration timeout = boost::posix_time::seconds(5);
    
    if (!(controlChannel->receiveRequest(timeout))) {
        std::cout << "Timeout while waiting for request" << std::endl;
    } else if (controlChannel->getMessage().code != HelperControlMessage::REQUEST_GET_DEVICE_SERIAL_NUMBER) {
        std::cout << "Unexpected request code: " << controlChannel->getMessage().code << std::endl;
    } else {
        controlChannel->getMessage().code = HelperControlMessage::RESPONSE_OK;
        controlChannel->getMessage().deviceSerialNumber = device.getSerialNumber();
        
        if (!(controlChannel->sendResponse(timeout))) {
            std::cout << "Timeout while sending response" << std::endl;
        }
    }
    
    return 0;
}




























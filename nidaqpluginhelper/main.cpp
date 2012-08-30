//
//  main.cpp
//  nidaqpluginhelper
//
//  Created by Christopher Stawarz on 8/24/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <cstdlib>
#include <iostream>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "NIDAQPluginHelper.h"


int main(int argc, const char * argv[])
{
    if (argc != 3) {
        std::cerr << argv[0] << " requires 2 arguments" << std::endl;
        return EXIT_FAILURE;
    }
    
    const char *deviceName = argv[1];
    const char *sharedMemoryName = argv[2];
    
    boost::interprocess::shared_memory_object sharedMemory(boost::interprocess::open_only,
                                                           sharedMemoryName,
                                                           boost::interprocess::read_write);
    boost::interprocess::mapped_region mappedRegion(sharedMemory, boost::interprocess::read_write);
    void *address = mappedRegion.get_address();
    HelperControlChannel &controlChannel = *(static_cast<HelperControlChannel *>(address));
    
    NIDAQPluginHelper helper(deviceName, controlChannel);
    bool success = helper.handleControlRequests();
    
    return (success ? EXIT_SUCCESS : EXIT_FAILURE);
}




























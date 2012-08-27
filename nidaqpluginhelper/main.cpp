//
//  main.cpp
//  nidaqpluginhelper
//
//  Created by Christopher Stawarz on 8/24/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <iostream>

#include "Error.h"
#include "Device.h"


int main(int argc, const char * argv[])
{
    mw::StandardServerCoreBuilder coreBuilder;
    mw::CoreBuilderForeman::constructCoreStandardOrder(&coreBuilder);
    
    try {
        
        nidaq::Device device("Dev1");
        
        std::cout << "Fetching serial number of device " << device.getName() << std::endl;
        uint32_t serialNumber = device.getSerialNumber();
        std::cout << "Serial number = " << std::hex << std::uppercase << serialNumber << std::dec << std::endl;
        
    } catch (nidaq::Error &e) {
        
        std::cout << "Caught nidaq::Error" << std::endl;
        std::cout << "  what:    " << e.what() << std::endl;
        std::cout << "  code:    " << e.getCode() << std::endl;
        std::cout << "  message: " << e.getMessage() << std::endl;
        
    } catch (std::exception &e) {
        
        std::cout << "Caught std::exception" << std::endl;
        std::cout << "  what: " << e.what() << std::endl;
        
    }
    
    return 0;
}

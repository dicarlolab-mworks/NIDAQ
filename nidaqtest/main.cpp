//
//  main.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <iostream>

#include "NIDAQError.h"
#include "NIDAQDevice.h"


int main(int argc, const char * argv[])
{
    NIDAQDevice device("Dev1");
    
    std::cout << "Fetching serial number of device " << device.getName() << std::endl;
    
    try {
        uInt32 serialNumber = device.getSerialNumber();
        std::cout << "Serial number = " << std::hex << std::uppercase << serialNumber;
    } catch (NIDAQError &e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << "Error message: " << e.what();
    }
    
    std::cout << std::endl;
    
    return 0;
}


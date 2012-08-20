//
//  main.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <iostream>
#include <string>

#include <NIDAQmxBase.h>


int main(int argc, const char * argv[])
{
    const char *device = "Dev1";
    
    std::cout << "Fetching serial number of device " << device << std::endl;

    uInt32 serialNumber;
    int32 error = DAQmxBaseGetDevSerialNum(device, &serialNumber);
    
    if (error == 0) {
        std::cout << "Serial number = " << std::hex << std::uppercase << serialNumber;
    } else {
        std::cout << "error = " << error;
    }
    std::cout << std::endl;
    
    return 0;
}


//
//  main.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>

#include <NIDAQmxBase.h>


int main(int argc, const char * argv[])
{
    const char *device = "Dev1";
    
    std::cout << "Fetching serial number of device " << device << std::endl;

    uInt32 serialNumber;
    int32 error = DAQmxBaseGetDevSerialNum(device, &serialNumber);
    
    if (!DAQmxFailed(error)) {
        std::cout << "Serial number = " << std::hex << std::uppercase << serialNumber;
    } else {
        std::cout << "Error code = " << error;
        
        uInt32 errorMessageSize = DAQmxBaseGetExtendedErrorInfo(NULL, 0);
        std::vector<char> errorMessage(errorMessageSize, 0);
        char *errorMessageBuffer = &(errorMessage.front());
        
        if (!DAQmxFailed(DAQmxBaseGetExtendedErrorInfo(errorMessageBuffer, errorMessageSize))) {
            std::cout << std::endl << "Error message: " << errorMessageBuffer;
        }
    }
    std::cout << std::endl;
    
    return 0;
}


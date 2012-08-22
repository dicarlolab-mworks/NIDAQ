//
//  main.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <cmath>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "NIDAQError.h"
#include "NIDAQDevice.h"
#include "NIDAQAnalogOutputTask.h"


int main(int argc, const char * argv[])
{
    try {
        
        NIDAQDevice device("Dev1");
        
        std::cout << "Fetching serial number of device " << device.getName() << std::endl;
        uint32_t serialNumber = device.getSerialNumber();
        std::cout << "Serial number = " << std::hex << std::uppercase << serialNumber << std::dec << std::endl;
        
        std::cout << "Creating task" << std::endl;
        NIDAQAnalogOutputTask task(device);
        
        const uint64_t samplesPerChan = 512;
        std::vector<double> data(2*samplesPerChan, 0.0);
        for (int i = 0; i < samplesPerChan; i++) {
            data[i] = 9.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
            data[i+samplesPerChan] = 0.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
        }
        
        std::cout << "Creating analog output channel" << std::endl;
        task.addVoltageChannel(0, -10.0, 10.0);
        std::cout << "Creating second analog output channel" << std::endl;
        task.addVoltageChannel(1, -1.0, 1.0);
        
        task.setSampleClockTiming(10000.0);
        int32_t sampsPerChanWritten = task.write(10.0, data);
        std::cout << "Wrote " << sampsPerChanWritten << " samples per channel to buffer" << std::endl;
        
        task.start();
        usleep(10000000);
        task.stop();
        
        std::cout << "Done!" << std::endl;
        
    } catch (NIDAQError &e) {
        
        std::cout << "Caught NIDAQError" << std::endl;
        std::cout << "  what:    " << e.what() << std::endl;
        std::cout << "  code:    " << e.getCode() << std::endl;
        std::cout << "  message: " << e.getMessage() << std::endl;
        
    } catch (std::exception &e) {
        
        std::cout << "Caught std::exception" << std::endl;
        std::cout << "  what: " << e.what() << std::endl;
        
    }
    
    return 0;
}



























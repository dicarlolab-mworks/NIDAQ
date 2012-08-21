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
#include "NIDAQTask.h"


int main(int argc, const char * argv[])
{
    try {
        
        NIDAQDevice device("Dev1");
        
        std::cout << "Fetching serial number of device " << device.getName() << std::endl;
        uInt32 serialNumber = device.getSerialNumber();
        std::cout << "Serial number = " << std::hex << std::uppercase << serialNumber << std::dec << std::endl;
        
        std::cout << "Creating task" << std::endl;
        NIDAQTask task(device, "AOTask");
        
        const uInt64 samplesPerChan = 512;
        std::vector<float64> data(2*samplesPerChan, 0.0);
        for (int i = 0; i < samplesPerChan; i++) {
            data[i] = 9.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
            data[i+samplesPerChan] = 0.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
        }
        
        std::cout << "Creating analog output channel" << std::endl;
        task.createAnalogOutputVoltageChannel("Dev1/ao0", -10.0, 10.0);
        std::cout << "Creating second analog output channel" << std::endl;
        task.createAnalogOutputVoltageChannel("Dev1/ao1", -1.0, 1.0);
        
        task.configureSampleClockTiming("", 10000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, samplesPerChan);
        int32 sampsPerChanWritten = task.writeAnalog(samplesPerChan, 10.0, DAQmx_Val_GroupByChannel, data);
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
        
    }
    
    return 0;
}

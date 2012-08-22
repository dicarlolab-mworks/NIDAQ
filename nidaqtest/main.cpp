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
#include "NIDAQAnalogInputTask.h"
#include "NIDAQAnalogOutputTask.h"


static void generateSineWaves(const NIDAQDevice &device) {
    std::cout << "Creating analog output task" << std::endl;
    NIDAQAnalogOutputTask task(device);
    
    std::cout << "Creating first analog output channel" << std::endl;
    task.addVoltageChannel(0, -10.0, 10.0);
    std::cout << "Creating second analog output channel" << std::endl;
    task.addVoltageChannel(1, -1.0, 1.0);
    
    std::cout << "Configuring sample clock timing" << std::endl;
    task.setSampleClockTiming(10000.0);
    
    const uint64_t samplesPerChan = 512;
    std::vector<double> data(2*samplesPerChan, 0.0);
    for (int i = 0; i < samplesPerChan; i++) {
        data[i] = 9.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
        data[i+samplesPerChan] = 0.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
    }
    
    std::cout << "Writing samples" << std::endl;
    int32_t sampsPerChanWritten = task.write(10.0, data);
    std::cout << "Wrote " << sampsPerChanWritten << " of " << samplesPerChan << " samples per channel" << std::endl;
    
    task.start();
    usleep(10000000);
    task.stop();
}


static void acquireNScans(const NIDAQDevice &device) {
    std::cout << "Creating analog input task" << std::endl;
    NIDAQAnalogInputTask task(device);

    std::cout << "Creating analog input channel" << std::endl;
    task.addVoltageChannel(0, -10.0, 10.0);
    
    std::vector<double> data(1000, 0.0);
    
    std::cout << "Configuring sample clock timing" << std::endl;
    task.setSampleClockTiming(1000.0, "OnboardClock", data.size());
    
    task.start();
    
    std::cout << "Reading samples" << std::endl;
    int32_t sampsPerChanRead = task.read(10.0, data);
    std::cout << "Read " << sampsPerChanRead << " of " << data.size() << " samples per channel" << std::endl;
    
    task.stop();
    
    for (int i = 0; i < 100; i++) {
        std::cout << "data[" << i << "] = " << data[i] << std::endl;
    }
}


int main(int argc, const char * argv[])
{
    try {
        
        NIDAQDevice device("Dev1");
        
        std::cout << "Fetching serial number of device " << device.getName() << std::endl;
        uint32_t serialNumber = device.getSerialNumber();
        std::cout << "Serial number = " << std::hex << std::uppercase << serialNumber << std::dec << std::endl;
        
        //generateSineWaves(device);
        acquireNScans(device);
        
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



























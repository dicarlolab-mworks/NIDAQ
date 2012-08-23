//
//  main.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <cmath>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "Error.h"
#include "Device.h"
#include "AnalogInputTask.h"
#include "AnalogOutputTask.h"


static void generateSineWaves(const nidaq::Device &device) {
    std::cout << "Creating analog output task" << std::endl;
    nidaq::AnalogOutputTask task(device);
    
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
    int32_t sampsPerChanWritten = task.write(data, 10.0);
    std::cout << "Wrote " << sampsPerChanWritten << " of " << samplesPerChan << " samples per channel" << std::endl;
    
    task.start();
    usleep(10000000);
    task.stop();
}


static void acquireNScans(const nidaq::Device &device) {
    std::cout << "Creating analog input task" << std::endl;
    nidaq::AnalogInputTask task(device);

    std::cout << "Creating analog input channel" << std::endl;
    task.addVoltageChannel(0, -10.0, 10.0);
    
    std::vector<double> data(1000, 0.0);
    
    std::cout << "Configuring sample clock timing" << std::endl;
    task.setSampleClockTiming(1000.0, "OnboardClock", data.size());
    
    task.start();
    
    std::cout << "Reading samples" << std::endl;
    int32_t sampsPerChanRead = task.read(data, 10.0);
    std::cout << "Read " << sampsPerChanRead << " of " << data.size() << " samples per channel" << std::endl;
    
    task.stop();
    
    for (int i = 0; i < 100; i++) {
        std::cout << "data[" << i << "] = " << data[i] << std::endl;
    }
}


static void analogRepeater(const nidaq::Device &device) {
    const double sampleRate = 10000.0;
    const double minVal = -10.0;
    const double maxVal = 10.0;
    const double runTime = 10.0;
    const double timeout = 10.0;
    
    std::vector<double> samples(1000, 0.0);
    
    // Analog input task
    std::cout << "Creating analog input task" << std::endl;
    nidaq::AnalogInputTask aiTask(device);
    aiTask.addVoltageChannel(0, minVal, maxVal);
    aiTask.setSampleClockTiming(10000.0);
    
    // Analog output task
    std::cout << "Creating analog output task" << std::endl;
    nidaq::AnalogOutputTask aoTask(device);
    aoTask.addVoltageChannel(0, minVal, maxVal);
    aoTask.setSampleClockTiming(10000.0);
    aoTask.setAllowRegeneration(false);
    
    aiTask.start();
    
    const size_t numIterations = size_t(sampleRate * runTime) / samples.size();
    for (size_t i = 0; i < numIterations; i++) {
        // Read input
        int32_t sampsPerChanRead = aiTask.read(samples, timeout);
        if (sampsPerChanRead != samples.size()) {
            std::cout << "Read only " << sampsPerChanRead << " of " << samples.size() << " samples per channel!" << std::endl;
        }
        
        // Write output
        int32_t sampsPerChanWritten = aoTask.write(samples, timeout);
        if (sampsPerChanWritten != samples.size()) {
            std::cout << "Wrote only " << sampsPerChanWritten << " of " << samples.size() << " samples per channel!" << std::endl;
        }
        
        if (!aoTask.isRunning()) {
            aoTask.start();
        }
    }
    
    aiTask.stop();
    aoTask.stop();
}


int main(int argc, const char * argv[])
{
    try {
        
        nidaq::Device device("Dev1");
        
        std::cout << "Fetching serial number of device " << device.getName() << std::endl;
        uint32_t serialNumber = device.getSerialNumber();
        std::cout << "Serial number = " << std::hex << std::uppercase << serialNumber << std::dec << std::endl;
        
        //generateSineWaves(device);
        //acquireNScans(device);
        analogRepeater(device);
        
        std::cout << "Done!" << std::endl;
        
    } catch (nidaq::Error &e) {
        
        std::cout << "Caught nidaq:Error" << std::endl;
        std::cout << "  what:    " << e.what() << std::endl;
        std::cout << "  code:    " << e.getCode() << std::endl;
        std::cout << "  message: " << e.getMessage() << std::endl;
        
    } catch (std::exception &e) {
        
        std::cout << "Caught std::exception" << std::endl;
        std::cout << "  what: " << e.what() << std::endl;
        
    }
    
    return 0;
}



























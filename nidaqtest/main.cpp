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

#include <boost/array.hpp>

#include "Error.h"
#include "Device.h"
#include "AnalogInputTask.h"
#include "AnalogOutputTask.h"

#include "MachTimer.h"


static void generateSineWaves(const nidaq::Device &device) {
    std::cout << "Creating analog output task" << std::endl;
    nidaq::AnalogOutputTask task(device);
    
    std::cout << "Creating first analog output channel" << std::endl;
    task.addVoltageChannel(0, -10.0, 10.0);
    std::cout << "Creating second analog output channel" << std::endl;
    task.addVoltageChannel(1, -1.0, 1.0);
    
    const size_t samplesPerChan = 512;
    std::vector<double> data(2*samplesPerChan, 0.0);
    for (size_t i = 0; i < samplesPerChan; i++) {
        data[i] = 9.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
        data[i+samplesPerChan] = 0.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
    }
    
    std::cout << "Configuring sample clock timing" << std::endl;
    task.setSampleClockTiming(10000.0, samplesPerChan);
    
    std::cout << "Writing samples" << std::endl;
    size_t samplesWritten = task.write(data, 10.0);
    std::cout << "Wrote " << samplesWritten << " of " << data.size() << " samples" << std::endl;
    
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
    task.setSampleClockTiming(1000.0, data.size(), false, "OnboardClock");
    
    task.start();
    
    std::cout << "Reading samples" << std::endl;
    size_t samplesRead = task.read(data, 10.0);
    std::cout << "Read " << samplesRead << " of " << data.size() << " samples" << std::endl;
    
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
    
    boost::array<double, 1000> samples;
    
    // Analog input task
    std::cout << "Creating analog input task" << std::endl;
    nidaq::AnalogInputTask aiTask(device);
    aiTask.addVoltageChannel(0, minVal, maxVal);
    aiTask.setSampleClockTiming(10000.0, samples.size());
    
    // Analog output task
    std::cout << "Creating analog output task" << std::endl;
    nidaq::AnalogOutputTask aoTask(device);
    aoTask.addVoltageChannel(0, minVal, maxVal);
    aoTask.setSampleClockTiming(10000.0, samples.size());
    aoTask.setAllowRegeneration(false);
    
    aiTask.start();
    
    const size_t numIterations = size_t(sampleRate * runTime) / samples.size();
    for (size_t i = 0; i < numIterations; i++) {
        // Read input
        size_t samplesRead = aiTask.read(samples, timeout);
        if (samplesRead != samples.size()) {
            std::cout << "Read only " << samplesRead << " of " << samples.size() << " samples!" << std::endl;
        }
        
        // Write output
        size_t samplesWritten = aoTask.write(samples, timeout);
        if (samplesWritten != samples.size()) {
            std::cout << "Wrote only " << samplesWritten << " of " << samples.size() << " samples!" << std::endl;
        }
        
        if (!aoTask.isRunning()) {
            aoTask.start();
        }
    }
    
    aiTask.stop();
    aoTask.stop();
}


static void testTiming(const nidaq::Device &device) {
    const double samplingRate = 10000.0;
    const double minVal = -10.0;
    const double maxVal = 10.0;
    const double timeout = 10.0;
    
    // Set up AO task
    
    nidaq::AnalogOutputTask aoTask(device);
    aoTask.addVoltageChannel(0, minVal, maxVal);
    
    boost::array<double, 100> aoSamples;
    for (size_t i = 0; i < aoSamples.size(); i++) {
        aoSamples[i] = 0.995 * maxVal * std::sin(double(i) * 2.0 * M_PI / double(aoSamples.size()));
    }
    
    aoTask.setSampleClockTiming(samplingRate, aoSamples.size());
    size_t samplesWritten = aoTask.write(aoSamples, timeout);
    if (samplesWritten != aoSamples.size()) {
        std::cout << "Wrote only " << samplesWritten << " of " << aoSamples.size() << " samples!" << std::endl;
        std::cout << "Aborting test" << std::endl;
        return;
    }
    
    // Set up AI task
    
    nidaq::AnalogInputTask aiTask(device);
    aiTask.addVoltageChannel(0, minVal, maxVal);
    
    boost::array<double, 1000> aiSamples;
    aiTask.setSampleClockTiming(samplingRate, aiSamples.size());
    
    // Collect data
    
    aoTask.start();
    
    MachTimer timer;
    aiTask.start();
    double startElapsed = timer.intervalMilli();
    size_t samplesRead = aiTask.read(aiSamples, timeout);
    double readElapsed = timer.intervalMilli();
    size_t samplesAvailable = aiTask.getNumSamplesAvailable();
    
    aiTask.stop();
    aoTask.stop();
    
    // Report results
    
    if (samplesRead != aiSamples.size()) {
        std::cout << "Read only " << samplesRead << " of " << aiSamples.size() << " samples!" << std::endl;
        std::cout << "Aborting test" << std::endl;
        return;
    }
    
    for (size_t i = 0; i < 100; i++) {
        std::cout << "aiSamples[" << i << "] = " << aiSamples[i] << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Sampling interval:    " << (1000.0 / samplingRate) << " ms" << std::endl;
    std::cout << "Samples read:         " << samplesRead << std::endl;
    std::cout << "Samples available:    " << samplesAvailable << std::endl;
    std::cout << "Time in start():      " << startElapsed << " ms" << std::endl;
    std::cout << "Time in read():       " << readElapsed << " ms" << std::endl;
    std::cout << "Read time per sample: " << (readElapsed / double(samplesRead)) << " ms" << std::endl;
    std::cout << std::endl;
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
        //analogRepeater(device);
        testTiming(device);
        
        std::cout << "Done!" << std::endl;
        
    } catch (const nidaq::Error &e) {
        
        std::cout << "Caught nidaq::Error" << std::endl;
        std::cout << "  what:    " << e.what() << std::endl;
        std::cout << "  code:    " << e.getCode() << std::endl;
        std::cout << "  message: " << e.getMessage() << std::endl;
        
    } catch (const std::exception &e) {
        
        std::cout << "Caught std::exception" << std::endl;
        std::cout << "  what: " << e.what() << std::endl;
        
    }
    
    return 0;
}



























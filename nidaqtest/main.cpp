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
#include "DigitalInputTask.h"
#include "DigitalOutputTask.h"

#include "MachTimer.h"


static void generateSineWaves(const nidaq::Device &device) {
    std::cout << "Creating analog output task" << std::endl;
    nidaq::AnalogOutputTask task(device);
    
    std::cout << "Creating first analog output channel" << std::endl;
    task.addVoltageChannel(0, -10.0, 10.0);
    std::cout << "Creating second analog output channel" << std::endl;
    task.addVoltageChannel(1, -1.0, 1.0);
    
    const std::size_t samplesPerChan = 512;
    std::vector<double> data(2*samplesPerChan, 0.0);
    for (std::size_t i = 0; i < samplesPerChan; i++) {
        data[i] = 9.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
        data[i+samplesPerChan] = 0.95 * std::sin(double(i) * 2.0 * M_PI / double(samplesPerChan));
    }
    
    std::cout << "Configuring sample clock timing" << std::endl;
    task.setSampleClockTiming(10000.0, samplesPerChan);
    
    std::cout << "Writing samples" << std::endl;
    std::size_t samplesWritten = task.write(data, 10.0);
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
    std::size_t samplesRead = task.read(data, 10.0);
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
    
    const std::size_t numIterations = std::size_t(sampleRate * runTime) / samples.size();
    for (std::size_t i = 0; i < numIterations; i++) {
        // Read input
        std::size_t samplesRead = aiTask.read(samples, timeout);
        if (samplesRead != samples.size()) {
            std::cout << "Read only " << samplesRead << " of " << samples.size() << " samples!" << std::endl;
        }
        
        // Write output
        std::size_t samplesWritten = aoTask.write(samples, timeout);
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
    for (std::size_t i = 0; i < aoSamples.size(); i++) {
        aoSamples[i] = 0.995 * maxVal * std::sin(double(i) * 2.0 * M_PI / double(aoSamples.size()));
    }
    
    aoTask.setSampleClockTiming(samplingRate, aoSamples.size());
    std::size_t samplesWritten = aoTask.write(aoSamples, timeout);
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
    std::size_t samplesRead = aiTask.read(aiSamples, timeout);
    double readElapsed = timer.intervalMilli();
    std::size_t samplesAvailable = aiTask.getNumSamplesAvailable();
    
    aiTask.stop();
    aoTask.stop();
    
    // Report results
    
    if (samplesRead != aiSamples.size()) {
        std::cout << "Read only " << samplesRead << " of " << aiSamples.size() << " samples!" << std::endl;
        std::cout << "Aborting test" << std::endl;
        return;
    }
    
    for (std::size_t i = 0; i < 100; i++) {
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


static inline void assertNumSamples(std::size_t expected, std::size_t actual) {
    if (actual != expected) {
        throw std::runtime_error("Wrong number of samples read or written");
    }
}


static void simpleDigitalIO(const nidaq::Device &device) {
    nidaq::DigitalOutputTask doTask(device);
    doTask.addChannel(0);
    
    nidaq::DigitalInputTask diTask(device);
    diTask.addChannel(1);
    
    boost::array<std::uint32_t, 1> outSamples, inSamples;
    const double timeout = 10.0;
    
    doTask.start();
    diTask.start();
    
    outSamples[0] = 0;
    assertNumSamples(1, doTask.write(outSamples, timeout));
    assertNumSamples(1, diTask.read(inSamples, timeout));
    std::cout << inSamples[0] << std::endl;
    
    outSamples[0] = 1;
    assertNumSamples(1, doTask.write(outSamples, timeout));
    assertNumSamples(1, diTask.read(inSamples, timeout));
    std::cout << inSamples[0] << std::endl;
    
    outSamples[0] = 0;
    assertNumSamples(1, doTask.write(outSamples, timeout));
    assertNumSamples(1, diTask.read(inSamples, timeout));
    std::cout << inSamples[0] << std::endl;
    
    diTask.stop();
    doTask.stop();
}


static void testDigitalIOLatency(const nidaq::Device &device) {
    nidaq::DigitalOutputTask doTask(device);
    doTask.addChannel(0);
    
    nidaq::DigitalInputTask diTask(device);
    diTask.addChannel(1);
    
    boost::array<std::uint32_t, 1> outSamples, inSamples;
    const double timeout = 10.0;
    
    doTask.start();
    diTask.start();
    
    const std::size_t numRepeats = 100;
    boost::array<double, numRepeats> times;
    outSamples[0] = 0;
    MachTimer timer;
    
    for (std::size_t i = 0; i < numRepeats; i++) {
        assertNumSamples(1, doTask.write(outSamples, timeout));
        assertNumSamples(1, diTask.read(inSamples, timeout));
        times[i] = timer.intervalMilli();
        
        if (inSamples[0] != outSamples[0]) {
            std::cout << "Sent " << outSamples[0] << " but received " << inSamples[0] << std::endl;
        }
        
        outSamples[0] = !outSamples[0];
    }
    
    diTask.stop();
    doTask.stop();
    
    for (std::size_t i = 0; i < numRepeats; i++) {
        std::cout << "Elapsed time: " << times[i] << " ms" << std::endl;
    }
}


int main(int argc, const char * argv[])
{
    try {
        
        nidaq::Device device("Dev1");
        
        std::cout << "Fetching serial number of device " << device.getName() << std::endl;
        std::uint32_t serialNumber = device.getSerialNumber();
        std::cout << "Serial number = " << std::hex << std::uppercase << serialNumber << std::dec << std::endl;
        
        //generateSineWaves(device);
        //acquireNScans(device);
        //analogRepeater(device);
        //testTiming(device);
        //simpleDigitalIO(device);
        testDigitalIOLatency(device);
        
        std::cout << "Done!" << std::endl;
        
    } catch (const nidaq::Error &e) {
        
        std::cout << "Caught nidaq::Error" << std::endl;
        std::cout << "  what:    " << e.what() << std::endl;
        std::cout << "  message: " << e.getMessage() << std::endl;
        std::cout << "  code:    " << e.getCode() << std::endl;
        
    } catch (const std::exception &e) {
        
        std::cout << "Caught std::exception" << std::endl;
        std::cout << "  what: " << e.what() << std::endl;
        
    }
    
    return 0;
}



























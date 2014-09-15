//
//  main.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <array>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <vector>

#include <boost/array.hpp>

#include "Error.h"
#include "Device.h"

#include "MachTimer.h"


static void generateSineWaves(nidaq::Device &device) {
    std::cout << "Creating analog output task" << std::endl;
    nidaq::AnalogOutputTask &task = device.getAnalogOutputTask();
    
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


static void variableOutputVoltage(nidaq::Device &device) {
    std::cout << "Creating analog output task" << std::endl;
    nidaq::AnalogOutputTask &task = device.getAnalogOutputTask();
    
    std::cout << "Creating analog output channel" << std::endl;
    task.addVoltageChannel(0, -10.0, 10.0);
    
    const std::size_t samplesPerChan = 1;
    std::array<double, samplesPerChan> data;
    
    task.start();
    
    for (std::size_t i = 1; i <= 5; i++) {
        data.fill(double(i));
        
        std::cout << "Writing samples" << std::endl;
        std::size_t samplesWritten = task.write(data, 10.0);
        std::cout << "Wrote " << samplesWritten << " of " << data.size() << " samples" << std::endl;
        
        usleep(5000000);
    }
    
    task.stop();
}


static void acquireNScans(nidaq::Device &device) {
    std::cout << "Creating analog input task" << std::endl;
    nidaq::AnalogInputTask &task = device.getAnalogInputTask();

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


static void analogRepeater(nidaq::Device &device) {
    const double sampleRate = 10000.0;
    const double minVal = -10.0;
    const double maxVal = 10.0;
    const double runTime = 10.0;
    const double timeout = 10.0;
    
    boost::array<double, 1000> samples;
    
    // Analog input task
    std::cout << "Creating analog input task" << std::endl;
    nidaq::AnalogInputTask &aiTask = device.getAnalogInputTask();
    aiTask.addVoltageChannel(0, minVal, maxVal);
    aiTask.setSampleClockTiming(10000.0, samples.size());
    
    // Analog output task
    std::cout << "Creating analog output task" << std::endl;
    nidaq::AnalogOutputTask &aoTask = device.getAnalogOutputTask();
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


static void testTiming(nidaq::Device &device) {
    const double samplingRate = 10000.0;
    const double minVal = -10.0;
    const double maxVal = 10.0;
    const double timeout = 10.0;
    
    // Set up AO task
    
    nidaq::AnalogOutputTask &aoTask = device.getAnalogOutputTask();
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
    
    nidaq::AnalogInputTask &aiTask = device.getAnalogInputTask();
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


static void simpleDigitalIO(nidaq::Device &device) {
    nidaq::DigitalOutputTask &doTask = device.getDigitalOutputTask(0);
    
    nidaq::DigitalInputTask &diTask = device.getDigitalInputTask();
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


static void testDigitalIOLatency(nidaq::Device &device) {
    nidaq::DigitalOutputTask &doTask = device.getDigitalOutputTask(0);
    
    nidaq::DigitalInputTask &diTask = device.getDigitalInputTask();
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


static void countEdges(nidaq::Device &device) {
    nidaq::CounterInputCountEdgesTask &ciTask = device.getCounterInputCountEdgesTask(0);
    
    const unsigned int interval = 5;
    const double timeout = 10.0;
    
    ciTask.start();
    
    MachTimer timer;
    sleep(interval);
    std::uint32_t count = ciTask.read(timeout);
    double elapsed = timer.intervalMilli();
    
    ciTask.stop();
    
    std::cout << "Counted " << count << " edges in " << elapsed / 1000.0 << " s" << std::endl;
}


int main(int argc, const char * argv[])
{
    try {
        
        std::cout << "Connecting to device..." << std::endl;
        nidaq::Device device("Dev1");
        
        std::cout << "Connected to device " << device.getName() << " (serial number "
                  << std::hex << std::uppercase << device.getSerialNumber() << std::dec <<  ")" << std::endl;
        
        //generateSineWaves(device);
        variableOutputVoltage(device);
        //acquireNScans(device);
        //analogRepeater(device);
        //testTiming(device);
        //simpleDigitalIO(device);
        //testDigitalIOLatency(device);
        //countEdges(device);
        
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



























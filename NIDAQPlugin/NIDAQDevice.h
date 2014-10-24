//
//  NIDAQDevice.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQDevice__
#define __NIDAQ__NIDAQDevice__

#include <map>
#include <vector>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#include <boost/interprocess/mapped_region.hpp>
#pragma clang diagnostic pop
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include "IPCRequestResponse.h"
#include "HelperSharedMemory.h"

#include "NIDAQAnalogInputVoltageChannel.h"
#include "NIDAQAnalogOutputVoltageChannel.h"
#include "NIDAQAnalogOutputVoltageWaveformChannel.h"
#include "NIDAQDigitalInputChannel.h"
#include "NIDAQDigitalOutputChannel.h"
#include "NIDAQCounterInputCountEdgesChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQDevice : public IODevice, boost::noncopyable {
    
public:
    static const std::string NAME;
    static const std::string UPDATE_INTERVAL;
    static const std::string ANALOG_INPUT_DATA_INTERVAL;
    static const std::string ANALOG_OUTPUT_DATA_INTERVAL;
    static const std::string ASSUME_MULTIPLEXED_ADC;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQDevice(const ParameterValueMap &parameters);
    ~NIDAQDevice();
    
    void addChild(std::map<std::string, std::string> parameters,
                  ComponentRegistryPtr reg,
                  boost::shared_ptr<Component> child) override;
    
    bool initialize() override;
    bool startDeviceIO() override;
    bool stopDeviceIO() override;
    
private:
    bool createTasks();
    void* readInput();
    
    bool haveAnalogInputChannels() const { return !(analogInputChannels.empty()); }
    bool createAnalogInputTask();
    bool startAnalogInputTask();
    void readAnalogInput();
    
    bool haveAnalogOutputVoltageChannels() const { return !(analogOutputVoltageChannels.empty()); }
    bool haveAnalogOutputVoltageWaveformChannels() const { return !(analogOutputVoltageWaveformChannels.empty()); }
    bool haveAnalogOutputChannels() const { return haveAnalogOutputVoltageChannels() || haveAnalogOutputVoltageWaveformChannels(); }
    bool createAnalogOutputTask();
    bool startAnalogOutputTask();
    bool writeAnalogOutput();
    
    bool haveDigitalInputChannels() const { return !(digitalInputChannels.empty()); }
    bool createDigitalInputTask();
    bool startDigitalInputTask();
    void readDigitalInput();
    
    bool haveDigitalOutputChannels() const { return !(digitalOutputChannels.empty()); }
    bool createDigitalOutputTasks();
    bool startDigitalOutputTasks();
    bool writeDigitalOutput(int portNumber);
    
    bool haveCounterInputCountEdgesChannels() const { return !(counterInputCountEdgesChannels.empty()); }
    bool createCounterInputCountEdgesTasks();
    bool startCounterInputCountEdgesTasks();
    void readEdgeCounts();
    
    void spawnHelper();
    void reapHelper();
    bool sendHelperRequest();
    
    const std::string deviceName;
    
    std::string requestSemName, responseSemName;
    IPCRequestResponse controlChannel;
    
    std::string sharedMemoryName;
    HelperSharedMemory sharedMemory;
    HelperControlMessage *controlMessage;
    
    pid_t helperPID;
    
    using scoped_lock = boost::recursive_mutex::scoped_lock;
    boost::recursive_mutex controlMutex;
    
    MWTime updateInterval;
    boost::shared_ptr<ScheduleTask> readInputScheduleTask;
    
    MWTime analogInputDataInterval;
    const bool assumeMultiplexedADC;
    std::vector< boost::shared_ptr<NIDAQAnalogInputVoltageChannel> > analogInputChannels;
    std::size_t analogInputSampleBufferSize;
    MWTime analogInputStartTime;
    std::uint64_t totalNumAnalogInputSamplesAcquired;
    bool analogInputTaskRunning;
    
    MWTime analogOutputDataInterval;
    std::vector< boost::shared_ptr<NIDAQAnalogOutputVoltageChannel> > analogOutputVoltageChannels;
    std::vector< boost::shared_ptr<NIDAQAnalogOutputVoltageWaveformChannel> > analogOutputVoltageWaveformChannels;
    std::size_t analogOutputSampleBufferSize;
    bool analogOutputTaskRunning;
    
    std::vector< boost::shared_ptr<NIDAQDigitalInputChannel> > digitalInputChannels;
    std::size_t digitalInputSampleBufferSize;
    bool digitalInputTaskRunning;
    
    typedef std::map< int, boost::shared_ptr<NIDAQDigitalOutputChannel> > DigitalOutputChannelMap;
    DigitalOutputChannelMap digitalOutputChannels;
    std::size_t digitalOutputSampleBufferSize;
    bool digitalOutputTasksRunning;
    
    typedef std::map< int, boost::shared_ptr<NIDAQCounterInputCountEdgesChannel> > CounterInputCountEdgesChannelMap;
    CounterInputCountEdgesChannelMap counterInputCountEdgesChannels;
    bool counterInputCountEdgesTasksRunning;
    
    
    class AnalogOutputVoltageNotification : public VariableNotification {
    public:
        explicit AnalogOutputVoltageNotification(const boost::shared_ptr<NIDAQDevice> &nidaqDevice) :
            nidaqDeviceWeak(nidaqDevice)
        { }
        
        void notify(const Datum &data, MWTime time) override;
        
    private:
        boost::weak_ptr<NIDAQDevice> nidaqDeviceWeak;
    };
    
    
    class DigitalOutputLineStateNotification : public VariableNotification {
    public:
        DigitalOutputLineStateNotification(const boost::shared_ptr<NIDAQDevice> &nidaqDevice, int portNumber) :
            nidaqDeviceWeak(nidaqDevice),
            portNumber(portNumber)
        { }
        
        void notify(const Datum &data, MWTime time) override;
        
    private:
        boost::weak_ptr<NIDAQDevice> nidaqDeviceWeak;
        const int portNumber;
    };
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQDevice__) */




























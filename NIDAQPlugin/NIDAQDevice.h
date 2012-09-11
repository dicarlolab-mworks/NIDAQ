//
//  NIDAQDevice.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQDevice__
#define __NIDAQ__NIDAQDevice__

#include <vector>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

#include "IPCRequestResponse.h"
#include "HelperSharedMemory.h"

#include "NIDAQAnalogInputVoltageChannel.h"


BEGIN_NAMESPACE_MW


class NIDAQDevice : public IODevice, boost::noncopyable {
    
public:
    static const std::string NAME;
    static const std::string ANALOG_INPUT_DATA_INTERVAL;
    static const std::string ANALOG_INPUT_UPDATE_INTERVAL;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQDevice(const ParameterValueMap &parameters);
    ~NIDAQDevice();
    
    void addChild(std::map<std::string, std::string> parameters,
                  ComponentRegistryPtr reg,
                  boost::shared_ptr<Component> child) MW_OVERRIDE;
    
    bool initialize() MW_OVERRIDE;
    bool startDeviceIO() MW_OVERRIDE;
    bool stopDeviceIO() MW_OVERRIDE;
    
private:
    bool createTasks();
    void* readAnalogInput();
    
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
    
    typedef boost::mutex::scoped_lock scoped_lock;
    boost::mutex controlMutex;
    
    MWTime analogInputDataInterval;
    MWTime analogInputUpdateInterval;
    std::vector< boost::shared_ptr<NIDAQAnalogInputVoltageChannel> > analogInputChannels;
    size_t analogInputSampleBufferSize;
    MWTime analogInputStartTime;
    boost::uint64_t totalNumAnalogInputSamplesAcquired;
    shared_ptr<ScheduleTask> analogInputScheduleTask;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQDevice__) */




























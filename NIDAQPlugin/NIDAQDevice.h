//
//  NIDAQDevice.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQDevice__
#define __NIDAQ__NIDAQDevice__

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "HelperControlMessage.h"


BEGIN_NAMESPACE_MW


class NIDAQDevice : public IODevice {
    
public:
    static const std::string NAME;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQDevice(const ParameterValueMap &parameters);
    ~NIDAQDevice();
    
    bool initialize() MW_OVERRIDE;
    
private:
    void createSharedMemory();
    void destroySharedMemory();
    
    void createControlChannel();
    void destroyControlChannel();
    
    void spawnHelper();
    void reapHelper();
    
    bool sendHelperRequest();
    
    const std::string deviceName;
    
    std::string sharedMemoryName;
    boost::interprocess::shared_memory_object sharedMemory;
    boost::interprocess::mapped_region mappedRegion;
    
    HelperControlChannel *controlChannel;
    
    pid_t helperPID;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQDevice__) */




























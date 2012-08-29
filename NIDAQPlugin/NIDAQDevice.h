//
//  NIDAQDevice.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__NIDAQDevice__
#define __NIDAQ__NIDAQDevice__

#include <boost/interprocess/shared_memory_object.hpp>


BEGIN_NAMESPACE_MW


class NIDAQDevice : public IODevice {
    
public:
    static const std::string NAME;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit NIDAQDevice(const ParameterValueMap &parameters);
    ~NIDAQDevice();
    
private:
    void createSharedMemory();
    void destroySharedMemory();
    
    void spawnHelper();
    void reapHelper();
    
    const std::string deviceName;
    
    std::string sharedMemoryName;
    boost::interprocess::shared_memory_object sharedMemory;
    
    pid_t helperPID;
    
};


END_NAMESPACE_MW


#endif /* !defined(__NIDAQ__NIDAQDevice__) */




























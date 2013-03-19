//
//  Device.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__Device__
#define __NIDAQ__Device__

#include <map>
#include <memory>
#include <set>
#include <string>

#include <boost/noncopyable.hpp>

#include "AnalogInputTask.h"
#include "AnalogOutputTask.h"
#include "DigitalInputTask.h"
#include "DigitalOutputTask.h"


BEGIN_NAMESPACE_NIDAQ


class Device : boost::noncopyable {
    
public:
    explicit Device(const std::string &name);
    ~Device();
    
    const std::string& getName() const {
        return name;
    }
    
    std::uint32_t getSerialNumber() const {
        return serialNumber;
    }
    
    AnalogInputTask& getAnalogInputTask();
    AnalogOutputTask& getAnalogOutputTask();
    DigitalInputTask& getDigitalInputTask();
    DigitalOutputTask& getDigitalOutputTask(unsigned int portNumber);
    
private:
    static std::uint32_t getSerialNumber(const std::string &deviceName);
    
    const std::string name;
    const std::uint32_t serialNumber;
    
    std::set<std::string> channelNames;
    
    std::unique_ptr<AnalogInputTask> analogInputTask;
    std::unique_ptr<AnalogOutputTask> analogOutputTask;
    std::unique_ptr<DigitalInputTask> digitalInputTask;
    std::map< unsigned int, std::unique_ptr<DigitalOutputTask> > digitalOutputTasks;
    
    friend class Task;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__Device__) */




























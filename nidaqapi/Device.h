//
//  Device.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __NIDAQ__Device__
#define __NIDAQ__Device__

#include <string>

#include <boost/noncopyable.hpp>

#include "Types.h"


BEGIN_NAMESPACE_NIDAQ


class Device : boost::noncopyable {
    
    class Task;
    class AnalogInputTask;
    class AnalogOutputTask;
    class DigitalInputTask;
    class DigitalOutputTask;
    
public:
    explicit Device(const std::string &name);
    ~Device();
    
    const std::string& getName() const {
        return name;
    }
    
    std::uint32_t getSerialNumber() const {
        return serialNumber;
    }
    
private:
    static std::uint32_t getSerialNumber(const std::string &deviceName);
    
    const std::string name;
    const std::uint32_t serialNumber;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__Device__) */




























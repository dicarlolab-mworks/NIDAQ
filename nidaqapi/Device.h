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

#include "Types.h"


BEGIN_NAMESPACE_NIDAQ


class Device {
    
public:
    explicit Device(const std::string &name) :
        name(name)
    { }
    
    const std::string& getName() const {
        return name;
    }
    
    std::uint32_t getSerialNumber() const;
    void reset() const;
    
private:
    const std::string name;
    
};


END_NAMESPACE_NIDAQ


#endif /* !defined(__NIDAQ__Device__) */

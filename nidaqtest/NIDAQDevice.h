//
//  NIDAQDevice.h
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __nidaqtest__NIDAQDevice__
#define __nidaqtest__NIDAQDevice__

#include <string>

#include <boost/noncopyable.hpp>

#include <NIDAQmxBase.h>


class NIDAQDevice : boost::noncopyable {
    
public:
    NIDAQDevice(const std::string &name) :
        name(name)
    { }
    
    const std::string& getName() const {
        return name;
    }
    
    uInt32 getSerialNumber() const;
    
private:
    const std::string name;
    
};


#endif /* !defined(__nidaqtest__NIDAQDevice__) */

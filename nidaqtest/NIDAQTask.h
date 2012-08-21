//
//  NIDAQTask.h
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef __nidaqtest__NIDAQTask__
#define __nidaqtest__NIDAQTask__

#include <string>

#include <boost/noncopyable.hpp>

#include <NIDAQmxBase.h>


class NIDAQDevice;  // Forward declaration


class NIDAQTask : boost::noncopyable {
    
public:
    virtual ~NIDAQTask();
    
    NIDAQTask(const std::string &name, const NIDAQDevice &device);
    
    void start();
    void stop();
    
private:
    TaskHandle handle;
    const std::string deviceName;
    bool running;
    
};


#endif /* !defined(__nidaqtest__NIDAQTask__) */

//
//  HelperSharedMemory.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/5/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef NIDAQ_HelperSharedMemory_h
#define NIDAQ_HelperSharedMemory_h

#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#include <boost/interprocess/mapped_region.hpp>
#pragma clang diagnostic pop
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/noncopyable.hpp>

#include "IPCRequestResponse.h"
#include "HelperControlMessage.h"


class HelperSharedMemory : boost::noncopyable {
    
public:
    HelperSharedMemory(boost::interprocess::create_only_t createOnly, const std::string &name) :
        sharedMemory(createOnly, name.c_str(), boost::interprocess::read_write),
        sharedMemoryRemover(name)
    { }
    
    HelperSharedMemory(boost::interprocess::open_only_t openOnly, const std::string &name) :
        sharedMemory(openOnly, name.c_str(), boost::interprocess::read_write)
    { }
    
    void setSize(std::size_t size) {
        sharedMemory.truncate(size);
    }
    
    HelperControlMessage* getMessagePtr() {
        mappedRegion = mapped_region(sharedMemory, boost::interprocess::read_write);
        return new(mappedRegion.get_address()) HelperControlMessage;
    }
    
private:
    typedef boost::interprocess::shared_memory_object shared_memory_object;
    typedef boost::interprocess::mapped_region mapped_region;
    
    shared_memory_object sharedMemory;
    IPCNamedResourceRemover<shared_memory_object> sharedMemoryRemover;
    mapped_region mappedRegion;
    
};


#endif /* !defined(NIDAQ_HelperSharedMemory_h) */




























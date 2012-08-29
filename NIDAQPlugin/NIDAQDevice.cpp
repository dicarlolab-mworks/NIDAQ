//
//  NIDAQDevice.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQDevice.h"

#include <cstring>
#include <spawn.h>
#include <sys/wait.h>

#include <openssl/rand.h>

#include <boost/format.hpp>


BEGIN_NAMESPACE_MW


const std::string NIDAQDevice::NAME("name");


void NIDAQDevice::describeComponent(ComponentInfo &info) {
    IODevice::describeComponent(info);
    
    info.setSignature("iodevice/nidaq");
    
    info.addParameter(NAME, true, "Dev1");
}


NIDAQDevice::NIDAQDevice(const ParameterValueMap &parameters) :
    IODevice(parameters),
    deviceName(parameters[NAME].str())
{
    createSharedMemory();
    createControlChannel();
    spawnHelper();
    
    boost::posix_time::time_duration timeout = boost::posix_time::seconds(10);
    controlChannel->getMessage().code = HelperControlMessage::REQUEST_GET_DEVICE_SERIAL_NUMBER;
    
    if (!(controlChannel->sendRequest(timeout) && controlChannel->receiveResponse(timeout))) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Timeout when contacting helper");
    }
    
    mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Device serial number = %X", controlChannel->getMessage().deviceSerialNumber);
}


NIDAQDevice::~NIDAQDevice() {
    reapHelper();
    destroyControlChannel();
    destroySharedMemory();
}


void NIDAQDevice::createSharedMemory() {
    boost::uint64_t uniqueID;
    
    if (RAND_pseudo_bytes(reinterpret_cast<unsigned char *>(&uniqueID), sizeof(uniqueID)) < 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Unable to generate random name for shared memory object");
    }
    
    sharedMemoryName = (boost::format("nidaqplugin_%x") % uniqueID).str();
    
    // Don't attempt to destroy the shared memory here, since it's possible (although very unlikely) that another
    // NIDAQDevice instance has already generated and used an identical sharedMemoryName.  In that case, it's better
    // just to fail and let the user reload the experiment.
    
    sharedMemory = boost::interprocess::shared_memory_object(boost::interprocess::create_only,
                                                             sharedMemoryName.c_str(),
                                                             boost::interprocess::read_write);
}


void NIDAQDevice::destroySharedMemory() {
    boost::interprocess::shared_memory_object::remove(sharedMemoryName.c_str());
}


void NIDAQDevice::createControlChannel() {
    sharedMemory.truncate(sizeof(HelperControlChannel));
    mappedRegion = boost::interprocess::mapped_region(sharedMemory, boost::interprocess::read_write);
    void *address = mappedRegion.get_address();
    controlChannel = new(address) HelperControlChannel;
}


void NIDAQDevice::destroyControlChannel() {
    controlChannel->~HelperControlChannel();
}


void NIDAQDevice::spawnHelper() {
    const char * const argv[] = { PLUGIN_HELPER_EXECUTABLE, deviceName.c_str(), sharedMemoryName.c_str(), 0 };
    
    int status = posix_spawn(&helperPID,
                             PLUGIN_HELPERS_DIR "/" PLUGIN_HELPER_EXECUTABLE,
                             NULL,
                             NULL,
                             const_cast<char * const *>(argv),
                             NULL);
    
    if (status != 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN,
                              "Unable to launch " PLUGIN_HELPER_EXECUTABLE,
                              std::strerror(status));
    }
}


void NIDAQDevice::reapHelper() {
    int stat;
    waitpid(helperPID, &stat, 0);
}


END_NAMESPACE_MW




























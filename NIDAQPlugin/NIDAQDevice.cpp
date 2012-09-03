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
    deviceName(parameters[NAME].str()),
    controlChannel(NULL),
    controlMessage(NULL),
    helperPID(-1)
{
    boost::uint64_t uniqueID;
    
    if (RAND_pseudo_bytes(reinterpret_cast<unsigned char *>(&uniqueID), sizeof(uniqueID)) < 0) {
        throw SimpleException(M_IODEVICE_MESSAGE_DOMAIN, "Unable to generate random name for shared memory object");
    }
    
    std::string hexUniqueID = (boost::format("%x") % uniqueID).str();
    wantRequestName = hexUniqueID + "_1";
    wantResponseName = hexUniqueID + "_2";
    sharedMemoryName = "nidaqplugin_" + hexUniqueID;
}


NIDAQDevice::~NIDAQDevice() {
    reapHelper();
    boost::interprocess::shared_memory_object::remove(sharedMemoryName.c_str());
    destroyControlChannel();
}


bool NIDAQDevice::initialize() {
    createControlChannel();
    createControlMessage();
    spawnHelper();
    
    mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Looking for NIDAQ device \"%s\"...", deviceName.c_str());
    
    controlMessage->code = HelperControlMessage::REQUEST_GET_DEVICE_SERIAL_NUMBER;
    if (!sendHelperRequest()) {
        return false;
    }
    
    mprintf(M_IODEVICE_MESSAGE_DOMAIN,
            "Connected to NIDAQ device \"%s\" (serial number = %X)",
            deviceName.c_str(),
            controlMessage->deviceSerialNumber);
    
    return true;
}


void NIDAQDevice::createControlChannel() {
    controlChannel = new IPCRequestResponse(boost::interprocess::create_only, wantRequestName, wantResponseName);
}


void NIDAQDevice::destroyControlChannel() {
    delete controlChannel;
    IPCRequestResponse::remove(wantRequestName, wantResponseName);
}


void NIDAQDevice::createControlMessage() {
    // Don't attempt to destroy the shared memory here, since it's possible (although very unlikely) that another
    // NIDAQDevice instance has already generated and used an identical sharedMemoryName.  In that case, it's better
    // just to fail and let the user reload the experiment.
    
    sharedMemory = boost::interprocess::shared_memory_object(boost::interprocess::create_only,
                                                             sharedMemoryName.c_str(),
                                                             boost::interprocess::read_write);
    sharedMemory.truncate(sizeof(HelperControlMessage));
    mappedRegion = boost::interprocess::mapped_region(sharedMemory, boost::interprocess::read_write);
    void *address = mappedRegion.get_address();
    controlMessage = new(address) HelperControlMessage;
}


void NIDAQDevice::spawnHelper() {
    const char * const argv[] = {
        PLUGIN_HELPER_EXECUTABLE,
        deviceName.c_str(),
        wantRequestName.c_str(),
        wantResponseName.c_str(),
        sharedMemoryName.c_str(),
        0
    };
    
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
    if (helperPID <= 0) {
        return;
    }
    
    controlMessage->code = HelperControlMessage::REQUEST_SHUTDOWN;
    sendHelperRequest();
    
    int status;
    if (-1 == waitpid(helperPID, &status, WUNTRACED)) {
        merror(M_IODEVICE_MESSAGE_DOMAIN,
               "Error while waiting for %s to exit: %s",
               PLUGIN_HELPER_EXECUTABLE,
               std::strerror(errno));
        return;
    }
    
    if (!WIFEXITED(status)) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN, "%s terminated abnormally", PLUGIN_HELPER_EXECUTABLE);
        return;
    }
    
    int exitStatus = WEXITSTATUS(status);
    if (exitStatus != 0) {
        mwarning(M_IODEVICE_MESSAGE_DOMAIN,
                 "%s returned non-zero exit status (%d)",
                 PLUGIN_HELPER_EXECUTABLE,
                 exitStatus);
    }
}


bool NIDAQDevice::sendHelperRequest() {
    controlChannel->postRequest();
    if (!(controlChannel->waitForResponse(boost::posix_time::seconds(10)))) {
        merror(M_IODEVICE_MESSAGE_DOMAIN, "Control request to %s timed out", PLUGIN_HELPER_EXECUTABLE);
        return false;
    }
    
    HelperControlMessage &m = *controlMessage;
    const HelperControlMessage::signed_int responseCode = m.code;
    
    switch (responseCode) {
        case HelperControlMessage::RESPONSE_OK:
            // Success
            break;
            
        case HelperControlMessage::RESPONSE_BAD_REQUEST:
            merror(M_IODEVICE_MESSAGE_DOMAIN,
                   "Internal error: Bad request to %s: %s",
                   PLUGIN_HELPER_EXECUTABLE,
                   m.badRequest.info.data());
            break;
            
        case HelperControlMessage::RESPONSE_NIDAQ_ERROR:
            merror(M_IODEVICE_MESSAGE_DOMAIN,
                   "NIDAQ error: %s (%d)",
                   m.nidaqError.message.data(),
                   m.nidaqError.code);
            break;
            
        case HelperControlMessage::RESPONSE_EXCEPTION:
            merror(M_IODEVICE_MESSAGE_DOMAIN,
                   "Internal error: Exception in %s: %s",
                   PLUGIN_HELPER_EXECUTABLE,
                   m.exception.what.data());
            break;
            
        default:
            merror(M_IODEVICE_MESSAGE_DOMAIN,
                   "Internal error: Unknown response code (%d) from %s",
                   responseCode,
                   PLUGIN_HELPER_EXECUTABLE);
            break;
    }
    
    return (responseCode == HelperControlMessage::RESPONSE_OK);
}


END_NAMESPACE_MW




























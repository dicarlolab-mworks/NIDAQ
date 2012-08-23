//
//  NIDAQDevice.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQDevice.h"


BEGIN_NAMESPACE_MW


const std::string NIDAQDevice::NAME("name");


void NIDAQDevice::describeComponent(ComponentInfo &info) {
    IODevice::describeComponent(info);
    
    info.setSignature("iodevice/nidaq");
    
    info.addParameter(NAME, true, "Dev1");
}


NIDAQDevice::NIDAQDevice(const ParameterValueMap &parameters) :
    IODevice(parameters),
    device(parameters[NAME].str())
{
    mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Looking for NIDAQ device \"%s\"...", device.getName().c_str());
    uint32_t serialNumber = device.getSerialNumber();
    mprintf(M_IODEVICE_MESSAGE_DOMAIN,
            "Connected to NIDAQ device \"%s\" (serial number = %X)",
            device.getName().c_str(),
            serialNumber);
}


END_NAMESPACE_MW

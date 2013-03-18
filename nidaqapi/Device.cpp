//
//  Device.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "Device.h"

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


Device::Device(const std::string &name) :
    name(name),
    serialNumber(getSerialNumber(name))
{ }


Device::~Device() {
    Error::logIfFailed(  DAQmxBaseResetDevice(name.c_str())  );
}


std::uint32_t Device::getSerialNumber(const std::string &deviceName) {
    nidaqmxbase::uint32_t serialNumber = 0;
    Error::throwIfFailed(  DAQmxBaseGetDevSerialNum(deviceName.c_str(), &serialNumber)  );
    return serialNumber;
}


END_NAMESPACE_NIDAQ




























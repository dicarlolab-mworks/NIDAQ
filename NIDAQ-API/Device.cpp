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


uint32_t Device::getSerialNumber() const {
    nidaqmxbase::uint32_t serialNumber;
    Error::throwIfFailed(  DAQmxBaseGetDevSerialNum(name.c_str(), &serialNumber)  );
    return serialNumber;
}


END_NAMESPACE_NIDAQ

//
//  NIDAQDevice.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQDevice.h"

#include "NIDAQError.h"


uInt32 NIDAQDevice::getSerialNumber() const {
    uInt32 serialNumber;
    NIDAQError::throwOnFailure(DAQmxBaseGetDevSerialNum(name.c_str(), &serialNumber));
    return serialNumber;
}

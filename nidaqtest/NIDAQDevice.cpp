//
//  NIDAQDevice.cpp
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/20/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQDevice.h"

#include "NIDAQmxBaseAPI.h"
#include "NIDAQError.h"


uint32_t NIDAQDevice::getSerialNumber() const {
    nidaqmxbase::uint32_t serialNumber;
    NIDAQError::throwIfFailed(  DAQmxBaseGetDevSerialNum(name.c_str(), &serialNumber)  );
    return serialNumber;
}

//
//  DigitalIOTask.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/12/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "DigitalIOTask.h"

#include <boost/format.hpp>

#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


void DigitalIOTask::addChannel(unsigned int portNumber) {
    std::string lines = (boost::format("%s/port%u") % getDeviceName() % portNumber).str();
    Error::throwIfFailed(  createChannel(lines)  );
    Task::addChannel();
}


END_NAMESPACE_NIDAQ

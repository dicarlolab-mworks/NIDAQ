//
//  MachError.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/10/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "MachError.h"

#include <mach/mach_error.h>

#include <boost/format.hpp>


MachError::MachError(mach_error_t value) :
    std::runtime_error((boost::format("Mach error: %s (%d)") % mach_error_string(value) % value).str()),
    value(value)
{ }

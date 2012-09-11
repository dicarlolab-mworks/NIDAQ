//
//  NIDAQPluginHelperError.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 9/3/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef NIDAQ_NIDAQPluginHelperError_h
#define NIDAQ_NIDAQPluginHelperError_h

#include <stdexcept>

#include <boost/format.hpp>


class NIDAQPluginHelperError : public std::logic_error {
    
public:
    explicit NIDAQPluginHelperError(const std::string &s) :
        std::logic_error(s)
    { }
    
    explicit NIDAQPluginHelperError(const boost::format &f) :
        std::logic_error(f.str())
    { }
    
};


#endif /* !defined(NIDAQ_NIDAQPluginHelperError_h) */

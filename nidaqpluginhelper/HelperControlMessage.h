//
//  HelperControlMessage.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef NIDAQ_HelperControlMessage_h
#define NIDAQ_HelperControlMessage_h

#include <cstring>
#include <string>

#include <boost/array.hpp>
#include <boost/cstdint.hpp>

#include "IPCRequestResponse.h"


template <std::size_t N>
class char_array : public boost::array<char, N> {
    
    typedef boost::array<char, N> base_type;
    
public:
    char_array& operator=(const std::string &s) {
        return (*this = s.c_str());
    }
    
    char_array& operator=(const char *str) {
        std::strncpy(base_type::c_array(), str, N);
        base_type::back() = '\0';  // In case str contains more than N-1 characters
        return (*this);
    }
    
};


struct HelperControlMessage {
    
    enum {
        // Request codes
        REQUEST_GET_DEVICE_SERIAL_NUMBER,
        REQUEST_SHUTDOWN,
        
        // Response codes
        RESPONSE_OK,
        RESPONSE_ERROR
    };
    
    int64_t code;
    
    union {
        
        /*
        struct {
            char_array<32> name;
        } createDeviceRequest;
         */
        
        boost::uint32_t deviceSerialNumber;
        char_array<1024> errorMessage;
        
    };
    
};


typedef IPCRequestResponse<HelperControlMessage> HelperControlChannel;


#endif /* !defined(NIDAQ_HelperControlMessage_h) */




























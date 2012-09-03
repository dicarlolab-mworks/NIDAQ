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
#include <boost/format.hpp>


template <std::size_t N>
class char_array : public boost::array<char, N> {
    
    typedef boost::array<char, N> base_type;
    
public:
    char_array& operator=(const boost::format &f) {
        return (*this = f.str());
    }
    
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
    
    typedef boost::int64_t signed_int;
    typedef boost::uint64_t unsigned_int;
    typedef char_array<1024> message_buffer;
    
    //
    // Message code
    //
    
    enum {
        // Request codes
        REQUEST_GET_DEVICE_SERIAL_NUMBER,
        REQUEST_SHUTDOWN,
        
        // Response codes
        RESPONSE_OK,
        RESPONSE_BAD_REQUEST,
        RESPONSE_NIDAQ_ERROR,
        RESPONSE_EXCEPTION
    };
    
    signed_int code;
    
    //
    // Message data
    //
    
    union {
        //
        // Response data
        //
        
        unsigned_int deviceSerialNumber;
        
        struct {
            // Description of invalid or unrecognized request
            message_buffer info;
        } badRequest;
        
        struct {
            // NIDAQmxBase error info
            signed_int code;
            message_buffer message;
        } nidaqError;
        
        struct {
            // what() from caught exception
            message_buffer what;
        } exception;
    };
    
} __attribute__((aligned (8)));;


#endif /* !defined(NIDAQ_HelperControlMessage_h) */




























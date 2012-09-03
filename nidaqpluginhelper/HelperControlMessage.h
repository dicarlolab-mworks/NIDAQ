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


struct HelperControlMessage {
    
    //
    // Type definitions (portable between x86_64 and i386)
    //
    
    template <std::size_t size>
    class string_buffer {
    public:
        string_buffer& operator=(const char *str) {
            std::strncpy(data.data(), str, size);
            data.back() = '\0';  // In case str contains more than size-1 characters
            return (*this);
        }
        
        string_buffer& operator=(const std::string &s) {
            return (*this = s.c_str());
        }
        
        string_buffer& operator=(const boost::format &f) {
            return (*this = f.str());
        }
        
        const char* c_str() const {
            return data.data();
        }
        
        std::string str() const {
            return std::string(c_str());
        }
        
    private:
        boost::array<char, size> data;
    };
    
    typedef boost::int64_t signed_int;
    typedef boost::uint64_t unsigned_int;
    typedef string_buffer<1024> message_buffer;
    
    //
    // Zeroing constructor
    //
    
    HelperControlMessage() {
        std::memset(this, 0, sizeof(*this));
    }
    
    //
    // Message code
    //
    
    enum {
        UNKNOWN_MESSAGE_CODE = 0,
        
        // Request codes
        REQUEST_GET_DEVICE_SERIAL_NUMBER,
        REQUEST_CREATE_ANALOG_INPUT_TASK,
        REQUEST_CREATE_ANALOG_INPUT_VOLTAGE_CHANNEL,
        REQUEST_SET_ANALOG_INPUT_SAMPLE_CLOCK_TIMING,
        REQUEST_START_ANALOG_INPUT_TASK,
        REQUEST_STOP_ANALOG_INPUT_TASK,
        REQUEST_SHUTDOWN,
        
        // Response codes
        RESPONSE_OK,
        RESPONSE_NIDAQ_ERROR,
        RESPONSE_EXCEPTION,
        RESPONSE_BAD_REQUEST
    };
    
    signed_int code;
    
    //
    // Message data
    //
    
    union {
        //
        // Request data
        //
        
        struct {
            unsigned_int channelNumber;
            double minVal;
            double maxVal;
        } createAnalogInputVoltageChannel;
        
        struct {
            double samplingRate;
        } setAnalogInputSampleClockTiming;
        
        //
        // Response data
        //
        
        // Device serial number
        unsigned_int deviceSerialNumber;
        
        // NIDAQmxBase error info
        struct {
            signed_int code;
            message_buffer message;
        } nidaqError;
        
        // what() from caught exception
        struct {
            message_buffer what;
        } exception;
        
        // Description of invalid or unrecognized request
        struct {
            message_buffer info;
        } badRequest;
    };
    
} __attribute__((aligned (8)));;


#endif /* !defined(NIDAQ_HelperControlMessage_h) */




























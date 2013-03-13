//
//  HelperControlMessage.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/29/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef NIDAQ_HelperControlMessage_h
#define NIDAQ_HelperControlMessage_h

#include <algorithm>
#include <cstring>
#include <string>

#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/format.hpp>

using std::size_t;


struct HelperControlMessage {
    
    //
    // Type definitions (portable between x86_64 and i386)
    //
    
    typedef boost::int64_t signed_int;
    typedef boost::uint64_t unsigned_int;
    
    template <typename SampleType>
    struct samples_buffer {
        size_t size() const {
            return size_t(numSamples);
        }
        
        const SampleType& operator[](size_t i) const {
            return *(&firstSample + i);
        }
        
        SampleType& operator[](size_t i) {
            return const_cast<SampleType &>(static_cast<const samples_buffer &>(*this)[i]);
        }
        
        unsigned_int numSamples;  // *not* size_t
        
    private:
        SampleType firstSample;
    };
    
    template <size_t bufferSize>
    class string_buffer {
    public:
        string_buffer& operator=(const char *str) {
            std::strncpy(data.data(), str, bufferSize);
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
        boost::array<char, bufferSize> data;
    };
    
    typedef string_buffer<1024> message_buffer;
    
    //
    // Message code
    //
    
    enum {
        UNKNOWN_MESSAGE_CODE = 0,
        
        //
        // Request codes
        //
        
        REQUEST_CREATE_ANALOG_INPUT_VOLTAGE_CHANNEL,
        REQUEST_SET_ANALOG_INPUT_SAMPLE_CLOCK_TIMING,
        REQUEST_START_ANALOG_INPUT_TASK,
        REQUEST_STOP_ANALOG_INPUT_TASK,
        REQUEST_READ_ANALOG_INPUT_SAMPLES,
        
        REQUEST_CREATE_ANALOG_OUTPUT_VOLTAGE_CHANNEL,
        REQUEST_SET_ANALOG_OUTPUT_SAMPLE_CLOCK_TIMING,
        REQUEST_START_ANALOG_OUTPUT_TASK,
        REQUEST_STOP_ANALOG_OUTPUT_TASK,
        REQUEST_WRITE_ANALOG_OUTPUT_SAMPLES,
        
        REQUEST_CREATE_DIGITAL_INPUT_CHANNEL,
        REQUEST_START_DIGITAL_INPUT_TASK,
        REQUEST_STOP_DIGITAL_INPUT_TASK,
        REQUEST_READ_DIGITAL_INPUT_SAMPLES,
        
        REQUEST_SHUTDOWN,
        REQUEST_PING,
        
        //
        // Response codes
        //
        
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
        } analogVoltageChannel;
        
        struct {
            double samplingRate;
            unsigned_int samplesPerChannelToAcquire;
        } sampleClockTiming;
        
        struct {
            unsigned_int portNumber;
        } digitalChannel;
        
        //
        // Request/response data
        //
        
        struct {
            double timeout;
            samples_buffer<double> samples;
        } analogSamples;
        
        struct {
            double timeout;
            samples_buffer<boost::uint32_t> samples;
        } digitalSamples;
        
        //
        // Response data
        //
        
        unsigned_int taskStartTime;
        
        struct {
            signed_int code;
            message_buffer what;
        } nidaqError;
        
        struct {
            message_buffer what;
        } exception;
        
        struct {
            message_buffer info;
        } badRequest;
    };
    
    //
    // Utility methods
    //
    
    static size_t sizeWithSamples(size_t numAnalogSamples, size_t numDigitalSamples) {
        HelperControlMessage m;
        size_t size = sizeof(m);
        char * const startAddress = reinterpret_cast<char *>(&m);
        
        size = std::max(size, sizeWithBuffer(startAddress, m.analogSamples.samples, numAnalogSamples));
        size = std::max(size, sizeWithBuffer(startAddress, m.digitalSamples.samples, numDigitalSamples));
        
        return size;
    }
    
private:
    template <typename SampleType>
    static size_t sizeWithBuffer(char * const startAddress,
                                 samples_buffer<SampleType> &samples,
                                 size_t numSamples)
    {
        char * const endAddress = reinterpret_cast<char *>(&(samples[0]) + numSamples);
        return size_t(endAddress - startAddress);
    }
    
} __attribute__((aligned (8)));;


#endif /* !defined(NIDAQ_HelperControlMessage_h) */




























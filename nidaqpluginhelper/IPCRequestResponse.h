//
//  IPCRequestResponse.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/28/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef NIDAQ_IPCRequestResponse_h
#define NIDAQ_IPCRequestResponse_h

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/type_traits/is_pod.hpp>

// Always use spinlock-based interprocess synchronization mechanisms, since the POSIX-based variants (which at
// present do not work correctly on OS X, but may someday) aren't portable between x86_64 and i386
#define BOOST_INTERPROCESS_FORCE_GENERIC_EMULATION

#include <boost/interprocess/sync/interprocess_semaphore.hpp>


#define ASSERT_SIZE_AND_ALIGNMENT(type, size, alignment) \
    BOOST_STATIC_ASSERT(sizeof(type) == (size) && boost::alignment_of<type>::value == (alignment))

// boost::is_pod can recognize POD classes and structs only if BOOST_IS_POD is defined
#ifdef BOOST_IS_POD
#  define ASSERT_IS_POD(type)  BOOST_STATIC_ASSERT(boost::is_pod<type>::value)
#else
#  include <boost/type_traits/is_class.hpp>
#  define ASSERT_IS_POD(type)  BOOST_STATIC_ASSERT(boost::is_pod<type>::value || boost::is_class<type>::value)
#endif


class IPCRequestResponseBase : boost::noncopyable {
    
public:
    typedef boost::posix_time::time_duration time_duration;
    
    IPCRequestResponseBase() :
        wantRequest(0),
        wantResponse(0)
    { }
    
    void sendRequest() {
        send(wantRequest);
    }
    
    bool receiveRequest(const time_duration &timeout) {
        return receive(wantRequest, timeout);
    }
    
    void sendResponse() {
        send(wantResponse);
    }
    
    bool receiveResponse(const time_duration &timeout) {
        return receive(wantResponse, timeout);
    }
    
private:
    typedef boost::interprocess::interprocess_semaphore interprocess_semaphore;
    typedef boost::posix_time::ptime ptime;
    typedef boost::date_time::microsec_clock<ptime> microsec_clock;
    
    void send(interprocess_semaphore &wantMessage) {
        wantMessage.post();
    }
    
    bool receive(interprocess_semaphore &wantMessage, const time_duration &timeout) {
        const ptime expirationTime = microsec_clock::universal_time() + timeout;
        return wantMessage.timed_wait(expirationTime);
    }
    
    interprocess_semaphore wantRequest, wantResponse;
    
    ASSERT_SIZE_AND_ALIGNMENT(interprocess_semaphore, 4, 4);
    
} __attribute__((aligned (8)));


template <typename MessageType>
class IPCRequestResponse : public IPCRequestResponseBase {
    
public:
    MessageType& getMessage() {
        return message;
    }
    
private:
    MessageType message;
    
    ASSERT_SIZE_AND_ALIGNMENT(IPCRequestResponseBase, 8, 8);
    ASSERT_IS_POD(MessageType);
    
};


#endif /* !defined(NIDAQ_IPCRequestResponse_h) */



























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

#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>


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
        requestAvailable(false),
        responseAvailable(false)
    { }
    
    bool sendRequest(const time_duration &timeout) {
        return send(requestAvailable, wantRequest, timeout);
    }
    
    bool receiveRequest(const time_duration &timeout) {
        return receive(requestAvailable, wantRequest, timeout);
    }
    
    bool sendResponse(const time_duration &timeout) {
        return send(responseAvailable, wantResponse, timeout);
    }
    
    bool receiveResponse(const time_duration &timeout) {
        return receive(responseAvailable, wantResponse, timeout);
    }
    
private:
    typedef boost::interprocess::interprocess_mutex interprocess_mutex;
    typedef boost::interprocess::interprocess_condition interprocess_condition;
    typedef boost::uint16_t flag;
    
    typedef boost::interprocess::scoped_lock<interprocess_mutex> scoped_lock;
    typedef boost::posix_time::ptime ptime;
    
    static ptime getExpirationTime(const time_duration &timeout) {
        return boost::date_time::microsec_clock<ptime>::universal_time() + timeout;
    }
    
    bool send(flag &messageAvailable, interprocess_condition &wantMessage, const time_duration &timeout) {
        scoped_lock lock(mutex, getExpirationTime(timeout));
        if (!lock) {
            return false;
        }
        
        messageAvailable = true;
        wantMessage.notify_one();
        return true;
    }
    
    bool receive(flag &messageAvailable, interprocess_condition &wantMessage, const time_duration &timeout) {
        ptime expirationTime = getExpirationTime(timeout);
        
        scoped_lock lock(mutex, expirationTime);
        if (!lock) {
            return false;
        }
        
        while (!messageAvailable) {
            if (!(wantMessage.timed_wait(lock, expirationTime))) {
                return false;
            }
        }
        
        messageAvailable = false;
        return true;
    }
    
    interprocess_mutex mutex;
    interprocess_condition wantRequest, wantResponse;
    flag requestAvailable, responseAvailable;
    
    ASSERT_SIZE_AND_ALIGNMENT(interprocess_mutex, 4, 4);
    ASSERT_SIZE_AND_ALIGNMENT(interprocess_condition, 12, 4);
    ASSERT_SIZE_AND_ALIGNMENT(flag, 2, 2);
    
} __attribute__((aligned (8)));


template <typename MessageType>
class IPCRequestResponse : public IPCRequestResponseBase {
    
public:
    MessageType& getMessage() {
        return message;
    }
    
private:
    MessageType message;
    
    ASSERT_SIZE_AND_ALIGNMENT(IPCRequestResponseBase, 32, 8);
    ASSERT_IS_POD(MessageType);
    
};


#endif /* !defined(NIDAQ_IPCRequestResponse_h) */



























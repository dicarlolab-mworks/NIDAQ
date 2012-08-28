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


class IPCRequestResponseBase : boost::noncopyable {
    
public:
    typedef boost::interprocess::interprocess_mutex interprocess_mutex;
    typedef boost::interprocess::interprocess_condition interprocess_condition;
    typedef boost::uint16_t flag;
    
    IPCRequestResponseBase() :
        requestAvailable(false),
        responseAvailable(false)
    { }
    
    interprocess_mutex mutex;
    interprocess_condition wantRequest, wantResponse;
    flag requestAvailable, responseAvailable;
    
private:
    ASSERT_SIZE_AND_ALIGNMENT(interprocess_mutex, 4, 4);
    ASSERT_SIZE_AND_ALIGNMENT(interprocess_condition, 12, 4);
    ASSERT_SIZE_AND_ALIGNMENT(flag, 2, 2);
    
} __attribute__((aligned (8)));


template <typename MessageType>
class IPCRequestResponse : private IPCRequestResponseBase {
    
public:
    MessageType& getMessage() {
        return message;
    }
    
    void sendRequest() {
        send(requestAvailable, wantRequest);
    }
    
    void receiveRequest() {
        receive(requestAvailable, wantRequest);
    }
    
    void sendResponse() {
        send(responseAvailable, wantResponse);
    }
    
    void receiveResponse() {
        receive(responseAvailable, wantResponse);
    }
    
private:
    typedef boost::interprocess::scoped_lock<interprocess_mutex> scoped_lock;
    
    void send(flag &messageAvailable, interprocess_condition &wantMessage) {
        scoped_lock lock(mutex);
        messageAvailable = true;
        wantMessage.notify_one();
    }
    
    void receive(flag &messageAvailable, interprocess_condition &wantMessage) {
        scoped_lock lock(mutex);
        while (!messageAvailable) {
            wantMessage.wait(lock);
        }
        messageAvailable = false;
    }
    
    MessageType message;
    
    ASSERT_SIZE_AND_ALIGNMENT(IPCRequestResponseBase, 32, 8);
    BOOST_STATIC_ASSERT(boost::is_pod<MessageType>::value);
    
};


#endif /* !defined(NIDAQ_IPCRequestResponse_h) */



























//
//  IPCRequestResponse.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/28/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef NIDAQ_IPCRequestResponse_h
#define NIDAQ_IPCRequestResponse_h

#include <string>

#include <boost/interprocess/sync/named_semaphore.hpp>

// We insist that boost::interprocess::named_semaphore use POSIX named semaphores, because the fallback
// implementation spins in wait() (meaning the helper process would use 100% of a CPU core just by sitting
// and waiting for the next request)
#ifndef BOOST_INTERPROCESS_NAMED_SEMAPHORE_USES_POSIX_SEMAPHORES
#  error POSIX named semaphores are not available
#endif


class IPCRequestResponse {
    
public:
    typedef boost::interprocess::create_only_t create_only_t;
    typedef boost::interprocess::open_only_t open_only_t;
    typedef boost::posix_time::time_duration time_duration;
    
    static void remove(const std::string &wantRequestName, const std::string &wantResponseName) {
        named_semaphore::remove(wantRequestName.c_str());
        named_semaphore::remove(wantResponseName.c_str());
    }
    
    IPCRequestResponse(create_only_t createOnly,
                       const std::string &wantRequestName,
                       const std::string &wantResponseName) :
        wantRequest(createOnly, wantRequestName.c_str(), 0),
        wantResponse(createOnly, wantResponseName.c_str(), 0)
    { }
    
    IPCRequestResponse(open_only_t openOnly,
                       const std::string &wantRequestName,
                       const std::string &wantResponseName) :
        wantRequest(openOnly, wantRequestName.c_str()),
        wantResponse(openOnly, wantResponseName.c_str())
    { }
    
    void postRequest() {
        post(wantRequest);
    }
    
    bool waitForRequest(const time_duration &timeout) {
        return wait(wantRequest, timeout);
    }
    
    void postResponse() {
        post(wantResponse);
    }
    
    bool waitForResponse(const time_duration &timeout) {
        return wait(wantResponse, timeout);
    }
    
private:
    typedef boost::interprocess::named_semaphore named_semaphore;
    typedef boost::posix_time::ptime ptime;
    typedef boost::date_time::microsec_clock<ptime> microsec_clock;
    
    void post(named_semaphore &wantMessage) {
        wantMessage.post();
    }
    
    bool wait(named_semaphore &wantMessage, const time_duration &timeout) {
#ifdef BOOST_INTERPROCESS_POSIX_TIMEOUTS
        const ptime expirationTime = microsec_clock::universal_time() + timeout;
        return wantMessage.timed_wait(expirationTime);
#else
        // timed_wait() would spin, so just ignore the timeout and do a regular wait
        wantMessage.wait();
        return true;
#endif
    }
    
    named_semaphore wantRequest, wantResponse;
    
};


#endif /* !defined(NIDAQ_IPCRequestResponse_h) */



























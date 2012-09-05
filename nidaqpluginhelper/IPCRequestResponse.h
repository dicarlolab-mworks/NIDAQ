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
#include <boost/noncopyable.hpp>

// We insist that boost::interprocess::named_semaphore use POSIX named semaphores, because the fallback
// implementation spins in wait() (meaning the helper process would use 100% of a CPU core just by sitting
// and waiting for the next request)
#ifndef BOOST_INTERPROCESS_NAMED_SEMAPHORE_USES_POSIX_SEMAPHORES
#  error POSIX named semaphores are not available
#endif


template <typename ResourceType>
class IPCNamedResourceRemover : boost::noncopyable {
    
public:
    IPCNamedResourceRemover() { }
    
    IPCNamedResourceRemover(const std::string &name) :
        name(name)
    { }
    
    ~IPCNamedResourceRemover() {
        if (!(name.empty())) {
            ResourceType::remove(name.c_str());
        }
    }
    
private:
    const std::string name;
    
};


class IPCRequestResponse {
    
public:
    typedef boost::posix_time::time_duration time_duration;
    
    IPCRequestResponse(boost::interprocess::create_only_t createOnly,
                       const std::string &requestSemName,
                       const std::string &responseSemName) :
        requestSem(createOnly, requestSemName.c_str(), 0),
        requestSemRemover(requestSemName),
        responseSem(createOnly, responseSemName.c_str(), 0),
        responseSemRemover(responseSemName)
    { }
    
    IPCRequestResponse(boost::interprocess::open_only_t openOnly,
                       const std::string &requestSemName,
                       const std::string &responseSemName) :
        requestSem(openOnly, requestSemName.c_str()),
        responseSem(openOnly, responseSemName.c_str())
    { }
    
    void postRequest() {
        post(requestSem);
    }
    
    bool waitForRequest(const time_duration &timeout) {
        return wait(requestSem, timeout);
    }
    
    void postResponse() {
        post(responseSem);
    }
    
    bool waitForResponse(const time_duration &timeout) {
        return wait(responseSem, timeout);
    }
    
private:
    typedef boost::interprocess::named_semaphore named_semaphore;
    typedef IPCNamedResourceRemover<named_semaphore> named_semaphore_remover;
    typedef boost::posix_time::ptime ptime;
    typedef boost::date_time::microsec_clock<ptime> microsec_clock;
    
    void post(named_semaphore &sem) {
        sem.post();
    }
    
    bool wait(named_semaphore &sem, const time_duration &timeout) {
#ifdef BOOST_INTERPROCESS_POSIX_TIMEOUTS
        const ptime expirationTime = microsec_clock::universal_time() + timeout;
        return sem.timed_wait(expirationTime);
#else
        // timed_wait() would spin, so just ignore the timeout and do a regular wait
        sem.wait();
        return true;
#endif
    }
    
    named_semaphore requestSem;
    named_semaphore_remover requestSemRemover;
    
    named_semaphore responseSem;
    named_semaphore_remover responseSemRemover;
    
};


#endif /* !defined(NIDAQ_IPCRequestResponse_h) */



























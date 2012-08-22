//
//  NIDAQmxBaseAPI.h
//  nidaqtest
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef nidaqtest_NIDAQmxBaseAPI_h
#define nidaqtest_NIDAQmxBaseAPI_h

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <NIDAQmxBase.h>


#define ASSERT_SAME_TYPE(T1, T2)  BOOST_STATIC_ASSERT((boost::is_same<T1, T2>::value))


namespace nidaqmxbase_type_assertions {
    // Verify that float32 and float64 are in fact float and double, respectively
    ASSERT_SAME_TYPE(float32, float);
    ASSERT_SAME_TYPE(float64, double);
    
    // Verify that TaskHandle is really unsigned long *
    ASSERT_SAME_TYPE(TaskHandle, unsigned long *);
}


#endif /* !defined(nidaqtest_NIDAQmxBaseAPI_h) */

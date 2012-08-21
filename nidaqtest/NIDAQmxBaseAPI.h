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
#include <boost/type_traits.hpp>

#include <NIDAQmxBase.h>


#define ASSERT_SAME_TYPE(T1, T2)  BOOST_STATIC_ASSERT((boost::is_same<T1, T2>::value))
#define ASSERT_SAME_SIZE(T1, T2)  BOOST_STATIC_ASSERT(sizeof(T1) == sizeof(T2))


namespace nidaqmxbase_type_assertions {
    ASSERT_SAME_TYPE(int8, signed char);
    ASSERT_SAME_TYPE(uInt8, unsigned char);
    ASSERT_SAME_TYPE(int16, short);
    ASSERT_SAME_TYPE(uInt16, unsigned short);
    ASSERT_SAME_TYPE(int32, long);
    ASSERT_SAME_TYPE(uInt32, unsigned long);
    ASSERT_SAME_TYPE(float32, float);
    ASSERT_SAME_TYPE(float64, double);
    ASSERT_SAME_TYPE(int64, long long);
    ASSERT_SAME_TYPE(uInt64, unsigned long long);
    
    // Use char instead of signed char
    BOOST_STATIC_ASSERT(boost::is_signed<char>::value);
    ASSERT_SAME_SIZE(signed char, char);
    
    // Use int instead of long
    ASSERT_SAME_SIZE(long, int);
    
    // Use unsigned int instead of unsigned long
    ASSERT_SAME_SIZE(unsigned long, unsigned int);
    
    // Use void* instead of TaskHandle
    BOOST_STATIC_ASSERT(boost::is_pointer<TaskHandle>::value);
    ASSERT_SAME_SIZE(TaskHandle, void*);
}


#endif /* !defined(nidaqtest_NIDAQmxBaseAPI_h) */

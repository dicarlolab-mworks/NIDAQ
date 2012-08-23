//
//  NIDAQmxBaseAPI.h
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/21/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef NIDAQ_NIDAQmxBaseAPI_h
#define NIDAQ_NIDAQmxBaseAPI_h

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <NIDAQmxBase.h>


#define ASSERT_SAME_TYPE(T1, T2)  BOOST_STATIC_ASSERT((boost::is_same<T1, T2>::value))


namespace nidaqmxbase_type_assertions {
    ASSERT_SAME_TYPE(float32, float);
    ASSERT_SAME_TYPE(float64, double);
    ASSERT_SAME_TYPE(TaskHandle, unsigned long *);
}


namespace nidaqmxbase {
    typedef int32 int32_t;
    typedef uInt32 uint32_t;
}


#endif /* !defined(NIDAQ_NIDAQmxBaseAPI_h) */

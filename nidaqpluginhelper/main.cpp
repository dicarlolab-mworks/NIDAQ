//
//  main.cpp
//  nidaqpluginhelper
//
//  Created by Christopher Stawarz on 8/24/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <cstdlib>
#include <iostream>
#include <thread>

#import <AppKit/AppKit.h>

#include "NIDAQPluginHelper.h"


int main(int argc, const char * argv[])
{
    if (argc != 5) {
        std::cerr << argv[0] << " requires 4 arguments" << std::endl;
        return EXIT_FAILURE;
    }
    
    const char *requestSemName = argv[1];
    const char *responseSemName = argv[2];
    const char *sharedMemoryName = argv[3];
    const char *deviceName = argv[4];
    
    IPCRequestResponse ipc(boost::interprocess::open_only, requestSemName, responseSemName);
    HelperSharedMemory sharedMemory(boost::interprocess::open_only, sharedMemoryName);
    bool success = false;
    std::atomic_bool running(true);
    
    //
    // Run the helper on a separate thread, so that we can execute the main run loop on
    // this thread and thereby process calls to
    // -[NSApplication nextEventMatchingMask:untilDate:inMode:dequeue:]
    //
    
    auto helperThread = std::thread([&ipc, &sharedMemory, &deviceName, &success, &running]() {
        NIDAQPluginHelper helper(ipc, sharedMemory, deviceName);
        success = helper.handleRequests();
        running = false;
    });
    
    @autoreleasepool {
        NSRunLoop *runLoop = NSRunLoop.currentRunLoop;
        while (running) {
            @autoreleasepool {
                [runLoop runUntilDate:[NSDate dateWithTimeIntervalSinceNow:1.0]];
            }
        }
    }
    
    helperThread.join();
    
    return (success ? EXIT_SUCCESS : EXIT_FAILURE);
}

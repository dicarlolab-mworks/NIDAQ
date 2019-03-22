//
//  NSApplication+NextEventMatchingMaskFix.m
//  nidaqpluginhelper
//
//  Created by Christopher Stawarz on 10/1/18.
//  Copyright © 2018 MIT. All rights reserved.
//

#import <AppKit/NSApplication.h>
#import <objc/runtime.h>


//
// The code in this file provides a workaround for the issue described at
// https://forums.ni.com/t5/LabVIEW/Labview-crashed-on-Mac-OS-X-Sierra-nextEventMatchingMask-should/td-p/3574639
//
// The method swizzling technique used here is taken from
// https://nshipster.com/method-swizzling/
//


@interface NSApplication (NextEventMatchingMaskFix)
@end


@implementation NSApplication (NextEventMatchingMaskFix)


+ (void)load {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        Class class = [self class];
        
        SEL originalSelector = @selector(nextEventMatchingMask:untilDate:inMode:dequeue:);
        SEL swizzledSelector = @selector(mwk_nidaq_nextEventMatchingMask:untilDate:inMode:dequeue:);
        
        Method originalMethod = class_getInstanceMethod(class, originalSelector);
        Method swizzledMethod = class_getInstanceMethod(class, swizzledSelector);
        
        BOOL didAddMethod = class_addMethod(class,
                                            originalSelector,
                                            method_getImplementation(swizzledMethod),
                                            method_getTypeEncoding(swizzledMethod));
        if (didAddMethod) {
            class_replaceMethod(class,
                                swizzledSelector,
                                method_getImplementation(originalMethod),
                                method_getTypeEncoding(originalMethod));
        } else {
            method_exchangeImplementations(originalMethod, swizzledMethod);
        }
    });
}


- (NSEvent *)mwk_nidaq_nextEventMatchingMask:(NSEventMask)mask
                                   untilDate:(NSDate *)expiration
                                      inMode:(NSRunLoopMode)mode
                                     dequeue:(BOOL)deqFlag
{
    if (NSThread.isMainThread) {
        return [self mwk_nidaq_nextEventMatchingMask:mask untilDate:expiration inMode:mode dequeue:deqFlag];
    }
    
    __block NSEvent *event = nil;
    dispatch_sync(dispatch_get_main_queue(), ^{
        event = [[self mwk_nidaq_nextEventMatchingMask:mask untilDate:expiration inMode:mode dequeue:deqFlag] retain];
    });
    return [event autorelease];
}


@end

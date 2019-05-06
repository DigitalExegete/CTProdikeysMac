//
//  CTProdikeysUSBHIDHandler.h
//  CTProdikeysMonitor
//
//  Created by William Jones on 12/2/18.
//  Copyright © 2018 Treblotto Music & Music Technology. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <IOKit/hid/IOHIDLib.h>

NS_ASSUME_NONNULL_BEGIN

@protocol CTProdikeysHIDDelegate <NSObject>

- (void)receivedHIDReport:(NSUInteger)reportID withReportData:(NSData *)reportData;
- (void)midiActiveStatusChanged:(BOOL)isActive;

@optional

@end

@interface CTProdikeysUSBHIDHandler : NSObject

@property (weak) id<CTProdikeysHIDDelegate> delegate;
@property (assign) IOHIDDeviceRef prodikeysHIDDevice;

- (void)initializeHID;

@end

NS_ASSUME_NONNULL_END

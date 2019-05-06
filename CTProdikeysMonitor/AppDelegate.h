//
//  AppDelegate.h
//  CTProdikeysMonitor
//
//  Created by William Jones on 12/2/18.
//  Copyright © 2018 Treblotto Music & Music Technology. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CTProdikeysUSBHIDHandler.h"

@interface AppDelegate : NSObject <NSApplicationDelegate, CTProdikeysHIDDelegate, NSMenuDelegate>

- (void)receivedHIDReport:(NSUInteger)reportID withReportData:(NSData *)reportData;

@end


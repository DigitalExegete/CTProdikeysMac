//
//  CTProdikeysUSBHIDHandler.m
//  CTProdikeysMonitor
//
//  Created by William Jones on 12/2/18.
//  Copyright © 2018 Treblotto Music & Music Technology. All rights reserved.
//

#import "CTProdikeysUSBHIDHandler.h"
#import <IOKit/IOKitLib.h>
#import <IOKit/hid/IOHIDManager.h>
#import <IOKit/hid/IOHIDKeys.h>


@interface CTProdikeysUSBHIDHandler()

@property NSMutableData *reportData;

- (void)handleConsumerReportRead:(uint8_t *)bytes;

- (void)handleMidiDataRead:(uint8_t *)bytes;

-(void)handleReport4Read:(uint8_t *)bytes;

- (void)handleSystemControlRead:(uint8_t *)bytes;

@end


static void CTProdikeysMatchCallback (
									  void * _Nullable        context,
									  IOReturn                result,
									  void * _Nullable        sender,
									  IOHIDDeviceRef          device);

static void CTProdikeysHIDReportReceived(void *context, IOReturn result, void *sender,
										 IOHIDReportType type, uint32_t reportID, uint8_t *report,
										 CFIndex reportLength);


static void match_set(CFMutableDictionaryRef dict, CFStringRef key, int value);

static CFDictionaryRef matching_dictionary_create(int vendorID,
												  int productID,
												  int usagePage,
												  int usage);


@implementation CTProdikeysUSBHIDHandler


- (void)initializeHID
{

	self.reportData = [[NSMutableData alloc] initWithLength:31]; //Max Report is 30 bytes + 1 for report ID.
	
	IOHIDManagerRef managerRef = IOHIDManagerCreate(
										kCFAllocatorDefault, kIOHIDOptionsTypeNone);
	
	IOHIDManagerOpen(managerRef, 0);
	
	IOHIDManagerRegisterDeviceMatchingCallback(
											   managerRef, CTProdikeysMatchCallback, (__bridge void *)self);
	
	CFRunLoopRef currentRLRef =  CFRunLoopGetCurrent();
	
	IOHIDManagerScheduleWithRunLoop(
									managerRef, currentRLRef, kCFRunLoopDefaultMode);
	
	CFDictionaryRef match = matching_dictionary_create(0x41E,0x2801, 0xFF00, 1);
	
	IOHIDManagerSetDeviceMatching(managerRef, match);
	
	CFRelease(match);
	
}

- (void)terminateHID
{
	
	
	
}

- (NSEvent *)otherEventForCode:(NSInteger)code keyDown:(BOOL)isKeyDown
{
	NSEvent *retEvent = nil;
	NSInteger modifierFlag = isKeyDown ? 0xa : 0xb;
	
	retEvent = [NSEvent otherEventWithType:NSEventTypeSystemDefined
								  location:NSZeroPoint
							 modifierFlags:(modifierFlag << 8)
								 timestamp:0
							  windowNumber:0
								   context:nil
								   subtype:8
									 data1:((code << 16) | (modifierFlag << 8))
									 data2:-1];
	
	return retEvent;
}

- (void)handleConsumerReportRead:(uint8_t *)bytes
{
	
	// 04 80 00 00 <= volume up
	// 04 00 10 00 <= volume down
	//#define NX_KEYTYPE_SOUND_UP		0
	//#define NX_KEYTYPE_SOUND_DOWN		1

	//let code = NX_KEYTYPE_ILLUMINATION_UP
	//let event1 = NSEvent.otherEvent(with: .systemDefined, location: NSPoint.zero, modifierFlags: NSEventModifierFlags(rawValue: 0xa00), timestamp: 0, windowNumber: 0, context: nil, subtype: 8, data1: (Int((code << 16 as Int32) | (0xa << 8 as Int32))), data2: -1)

	/*
	 #define NX_KEYTYPE_PLAY			16
	 #define NX_KEYTYPE_NEXT			17
	 #define NX_KEYTYPE_PREVIOUS		18
	 #define NX_KEYTYPE_FAST			19
	 #define NX_KEYTYPE_REWIND		20
	 */
	
	
	//MARK: Volume Events
	if ((bytes[1] & 0x80) == 0x80)
	{
		NSUInteger code = NX_KEYTYPE_SOUND_UP;
		NSEvent* keyEvent = [self otherEventForCode:code keyDown:YES];

		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
		keyEvent = [self otherEventForCode:code keyDown:NO];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
	}
	
	if ((bytes[2] & 0x01) == 0x01)
	{
		NSUInteger code = NX_KEYTYPE_SOUND_DOWN;
		NSEvent* keyEvent = [self otherEventForCode:code keyDown:YES];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
		keyEvent = [self otherEventForCode:code keyDown:NO];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
	}

	if ((bytes[1] & 0x10) == 0x10)
	{
		NSUInteger code = NX_KEYTYPE_MUTE;
		NSEvent* keyEvent = [self otherEventForCode:code keyDown:YES];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
		keyEvent = [self otherEventForCode:code keyDown:NO];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
	}
	
	//MARK: Transport/Music Playback Events
	
	if ((bytes[1] & 0x01) == 0x01)
	{
		NSUInteger code = NX_KEYTYPE_NEXT;
		NSEvent* keyEvent = [self otherEventForCode:code keyDown:YES];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
		keyEvent = [self otherEventForCode:code keyDown:NO];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
	}
	
	if ((bytes[1] & 0x02) == 0x02)
	{
		NSUInteger code = NX_KEYTYPE_PREVIOUS;
		NSEvent* keyEvent = [self otherEventForCode:code keyDown:YES];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
		keyEvent = [self otherEventForCode:code keyDown:NO];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
	}
	
	if ((bytes[1] & 0x08) == 0x08)
	{
		NSUInteger code = NX_KEYTYPE_PLAY;
		NSEvent* keyEvent = [self otherEventForCode:code keyDown:YES];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
		keyEvent = [self otherEventForCode:code keyDown:NO];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
	}
	
	if ((bytes[1] & 0x04) == 0x04)
	{
		NSUInteger code = NX_SUBTYPE_EJECT_KEY;
		NSEvent* keyEvent = [self otherEventForCode:code keyDown:YES];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
		keyEvent = [self otherEventForCode:code keyDown:NO];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
	}
	
	
	//MARK: Application Events
	if ((bytes[2] & 0x80) == 0x80) //Calculator
	{
		[[NSWorkspace sharedWorkspace] launchApplication:@"Calculator"];
	}

	if ((bytes[2] & 0x40) == 0x40) //E-mail
	{
		//TODO: Make this customizable
		if (![[NSWorkspace sharedWorkspace] launchApplication:@"Microsoft Outlook"])
		{
			[[NSWorkspace sharedWorkspace] launchApplication:@"Mail"];
		}
		
	}

	if ((bytes[3] & 0x04) == 0x04) //Internet Browser
	{
		[[NSWorkspace sharedWorkspace] launchApplication:@"Google Chrome"];
	}

	
	
	

	
}

- (void)handleMidiDataRead:(uint8_t *)bytes
{
	
}

-(void)handleReport4Read:(uint8_t *)bytes
{
	
	if ((bytes[1] & 0x2) == 0x2)
	{
		BOOL midiActive = (bytes[3] & 0x10) == 0x10;
		[[self delegate] midiActiveStatusChanged:midiActive];
	}
	
	
}

- (void)handleSystemControlRead:(uint8_t *)bytes
{
	
	if ((bytes[1] & 0x02) == 0x02)
	{
		NSUInteger code = NX_POWER_KEY;
		NSEvent* keyEvent = [self otherEventForCode:code keyDown:YES];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
		
		keyEvent = [self otherEventForCode:code keyDown:NO];
		
		CGEventPost(kCGHIDEventTap, [keyEvent CGEvent]);
	}

	
}

@end

static void CTProdikeysMatchCallback (
									  void * _Nullable        context,
									  IOReturn                result,
									  void * _Nullable        sender,
									  IOHIDDeviceRef          device)
{
	
	IOReturn r = IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone);
	if (r == kIOReturnSuccess)
	{
		CTProdikeysUSBHIDHandler *hidHandler = (__bridge CTProdikeysUSBHIDHandler *)context;
		
		[hidHandler setProdikeysHIDDevice:device];
		
		CFTypeRef activatedValue = IOHIDDeviceGetProperty(device, CFSTR("isMidiActive"));
		
		if (CFGetTypeID(activatedValue)==CFBooleanGetTypeID())
		{
			CFBooleanRef midiActivated = (CFBooleanRef)activatedValue;
			NSNumber *isMidiActive = (__bridge NSNumber *)midiActivated;
			
			[[hidHandler delegate] midiActiveStatusChanged:[isMidiActive boolValue]];
			
		}
		
		IOHIDDeviceRegisterInputReportCallback(
											   device,
											   [[hidHandler reportData] mutableBytes],
											   [[hidHandler reportData] length],
											   CTProdikeysHIDReportReceived,
											   (__bridge void*)hidHandler);
		
	}

	
}


static void match_set(CFMutableDictionaryRef dict, CFStringRef key, int value) {
	CFNumberRef number = CFNumberCreate(
										kCFAllocatorDefault, kCFNumberIntType, &value);
	CFDictionarySetValue(dict, key, number);
	CFRelease(number);
}

static CFDictionaryRef matching_dictionary_create(int vendorID,
												  int productID,
												  int usagePage,
												  int usage)
{
	CFMutableDictionaryRef match = CFDictionaryCreateMutable(
															 kCFAllocatorDefault, 0,
															 &kCFTypeDictionaryKeyCallBacks,
															 &kCFTypeDictionaryValueCallBacks);
	
	if (vendorID) {
		match_set(match, CFSTR(kIOHIDVendorIDKey), vendorID);
	}
	if (productID) {
		match_set(match, CFSTR(kIOHIDProductIDKey), productID);
	}
	if (usagePage) {
		match_set(match, CFSTR(kIOHIDDeviceUsagePageKey), usagePage);
	}
	if (usage) {
		match_set(match, CFSTR(kIOHIDDeviceUsageKey), usage);
	}
	
	return match;
}




static void CTProdikeysHIDReportReceived(void *context, IOReturn result, void *sender,
										 IOHIDReportType type, uint32_t reportID, uint8_t *report,
										 CFIndex reportLength)
{
	
	CTProdikeysUSBHIDHandler *handler = (__bridge CTProdikeysUSBHIDHandler *)context;

	[[handler delegate] receivedHIDReport:reportID withReportData:[NSData dataWithBytes:report length:reportLength]];
	
	switch (report[0])
	{
		case 1:
			[handler handleConsumerReportRead:report];
			break;
		case 2:
			[handler handleSystemControlRead:report];
			break;
		case 3:
			break;
		case 4:
			[handler handleReport4Read:report];
			break;
		default:
			break;
	}
	
}


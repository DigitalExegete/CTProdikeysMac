//
//  AppDelegate.m
//  CTProdikeysMonitor
//
//  Created by William Jones on 12/2/18.
//  Copyright © 2018 Treblotto Music & Music Technology. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSTextField *HIDReportField;
@property (strong) CTProdikeysUSBHIDHandler *hidHandler;
@property (strong) NSOperationQueue *deviceQueue;
@property (strong) NSStatusItem *monitorItem;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application
	
	self.hidHandler = [CTProdikeysUSBHIDHandler new];
	self.hidHandler.delegate = self;
	
	[self.hidHandler initializeHID];
	
	self.monitorItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
	self.monitorItem.button.image = [NSImage imageNamed:@"StatusBarIcon"];
	
	self.monitorItem.menu.delegate = self;
	
	self.monitorItem.button.target = self;
	self.monitorItem.button.action = @selector(userClickedStatusBarItem:);
	
	
}

- (IBAction)userClickedStatusBarItem:(id)sender
{
	
	BOOL visible = ![[self window] isVisible];
	
	[[self window] setIsVisible:visible];

	[NSApp setActivationPolicy: visible ? NSApplicationActivationPolicyRegular : NSApplicationActivationPolicyAccessory];
	
	if (visible)
	{
		[NSApp activateIgnoringOtherApps:YES];
		[[self window] makeKeyAndOrderFront:self];
	}
	
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	// Insert code here to tear down your application
	
	
	
}

- (void)receivedHIDReport:(NSUInteger)reportID withReportData:(NSData *)reportData
{
	
	[[self HIDReportField] setStringValue:[NSString stringWithFormat:@"Report %lu: %@", reportID, reportData]];
	
}

- (void)midiActiveStatusChanged:(BOOL)isActive
{
	
	NSImage *imageToUse = [NSImage imageNamed:@"StatusBarIcon"];
	
	if (isActive)
	{
		NSSize size = [imageToUse size];
		NSRect imageBounds = NSMakeRect(0, 0, size.width, size.height);
		
		[imageToUse setTemplate:NO];
		[imageToUse lockFocus];
		
		[[NSColor colorWithRed:0 green:0.75 blue:0 alpha:1] set];
		NSRectFillUsingOperation(imageBounds, NSCompositingOperationSourceAtop);
		
		[imageToUse unlockFocus];
	}
	else
	{
		[imageToUse setTemplate:YES];
	}
	
	[[self.monitorItem button] setImage:imageToUse];
	
}


@end

//
//  USBHIDMIDIDevice.hpp
//  GenericUSBMidi
//
//  Created by William Jones on 11/22/18.
//  Copyright © 2018 Yves Salathe. All rights reserved.
//

#ifndef USBHIDMIDIDevice_hpp
#define USBHIDMIDIDevice_hpp

#include <stdio.h>
#include <CoreMIDI/CoreMIDI.h>
#include "MIDIDriverDevice.h"
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/usb/USB.h>
#include <IOKit/hid/IOHIDLib.h>

class HIDMIDIDriverBase;
class HIDDevice;

class HIDMIDIDevice : public MIDIDriverDevice {
public:
	HIDMIDIDevice(	HIDMIDIDriverBase *	driver,
				  	HIDDevice * device,
				  	MIDIDeviceRef		midiDevice);
	
	virtual			~HIDMIDIDevice();
	
	// we have two-stage construction, so that overridden virtual methods
	// are correctly dispatched to subclasses
	virtual bool	Initialize();
	// return true for success
	
	
	
	// Leave data members public, so they may be accessed directly by driver methods.
	HIDMIDIDriverBase *			mDriver;
	HIDDevice *					mDevice;
	bool						mShuttingDown;
	
	enum { kNumReadBufs = 2, kNumWriteBufs = 1 };
	
	IOBuffer					mReadBuf[kNumReadBufs], mWriteBuf[kNumWriteBufs];
	int							mCurWriteBuf;
};


#endif /* USBHIDMIDIDevice_hpp */

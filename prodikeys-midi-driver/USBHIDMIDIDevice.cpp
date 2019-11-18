//
//  USBHIDMIDIDevice.cpp
//  GenericUSBMidi
//
//  Created by William Jones on 11/22/18.
//  Copyright © 2018 Yves Salathe. All rights reserved.
//

#include "USBHIDMIDIDevice.hpp"
#include "HIDDevice.h"

HIDMIDIDevice::HIDMIDIDevice(	HIDMIDIDriverBase *	driver,
							 HIDDevice * device,
							 MIDIDeviceRef		midiDevice) :
MIDIDriverDevice(midiDevice),
mDriver(driver),
mDevice(device)

{
	
	
	
}


HIDMIDIDevice::~HIDMIDIDevice()
{
	
}

bool HIDMIDIDevice::Initialize()
{

	SetUpEndpoints(true);
	return true;
	
}

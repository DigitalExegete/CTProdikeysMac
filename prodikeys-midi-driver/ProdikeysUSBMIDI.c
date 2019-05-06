//
//  ProdikeysUSBMIDI.c
//  prodikeys-midi-driver
//
//  Created by William Jones on 11/22/18.
//  Copyright © 2018 Treblotto Music & Music Technology. All rights reserved.
//

#include "ProdikeysUSBMIDI.h"


void *NewGenericUSBMidiDriver(CFAllocatorRef allocator, CFUUIDRef typeID);

void *NewGenericUSBMidiDriver(CFAllocatorRef allocator, CFUUIDRef typeID)
{
	// If correct type is being requested, allocate an
	// instance of TestType and return the IUnknown interface.
	if (CFEqual(typeID, kMIDIDriverTypeID)) {

		return NULL;
	} else {
		// If the requested type is incorrect, return NULL.
		return NULL;
	}
	
	
	
	
	
}

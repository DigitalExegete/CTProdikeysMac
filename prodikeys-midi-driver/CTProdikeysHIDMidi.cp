/*	Copyright � 2007 Apple Inc. All Rights Reserved.
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by
 Apple Inc. ("Apple") in consideration of your agreement to the
 following terms, and your use, installation, modification or
 redistribution of this Apple software constitutes acceptance of these
 terms.  If you do not agree with these terms, please do not use,
 install, modify or redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc.
 may be used to endorse or promote products derived from the Apple
 Software without specific prior written permission from Apple.  Except
 as expressly stated in this notice, no other rights or licenses, express
 or implied, are granted by Apple herein, including but not limited to
 any patent rights that may be infringed by your derivative works or by
 other works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */
#include "CTProdikeysHIDMidi.h"


// ^^^^^^^^^^^^^^^^^^ THINGS YOU MUST CUSTOMIZE ^^^^^^^^^^^^^^^^^^^^^^

// Generated UUID: 66ebf38d-f7a5-42f5-9f1e-742b35a4fbb6
#define kFactoryUUID CFUUIDGetConstantUUIDWithBytes(NULL, 0x66, 0xeb, 0xF3, 0x8d, 0xF7, 0xA5, 0x42, 0xF5, 0x9F, 0x1E, 0x74, 0x2B, 0x35, 0xA4, 0xFB, 0xB6)

#define kTheInterfaceToUse	1		// The Second one

#define kMyVendorID			0x041E	// Creative Technology
#define kMyProductID		0x2801
#define kMyNumPorts			1		// One in-out pair.

#define BOTTOM_F_DISTANCE_FROM_MIDDLE_C 0x14
#define OCTAVEOFFSET(X) ((X+2)*12)-BOTTOM_F_DISTANCE_FROM_MIDDLE_C


// __________________________________________________________________________________________________


// Implementation of the factory function for this type.
extern "C" void *NewCTProdikeysMidiDriver(CFAllocatorRef allocator, CFUUIDRef typeID);
extern "C" void *NewCTProdikeysMidiDriver(CFAllocatorRef allocator, CFUUIDRef typeID)
{
	// If correct type is being requested, allocate an
	// instance of TestType and return the IUnknown interface.
	if (CFEqual(typeID, kMIDIDriverTypeID)) {
		CTProdikeysHIDMidiDriver *result = new CTProdikeysHIDMidiDriver;
		return result->Self();
	} else {
		// If the requested type is incorrect, return NULL.
		return NULL;
	}
}




// __________________________________________________________________________________________________

CTProdikeysHIDMidiDriver::CTProdikeysHIDMidiDriver() :
HIDVendorMIDIDriver(kFactoryUUID), mHIDManagerRef(NULL), mCurrentOctave(3), sustainPedalDown(false)
{
}

CTProdikeysHIDMidiDriver::~CTProdikeysHIDMidiDriver()
{

	if (this->mHIDDevice)
	{
		delete mHIDDevice;
	}
	
	if (this->mReportBuffer)
	{
		delete mReportBuffer;
	}
	
	if (this->mProdikeysDevice)
	{
		delete mProdikeysDevice;
	}
}

// __________________________________________________________________________________________________


OSStatus CTProdikeysHIDMidiDriver::Start(MIDIDeviceListRef devList)
{
	
	
	ByteCount midi_devs = MIDIDeviceListGetNumberOfDevices(devList);
	
	for (int dev_index = 0; dev_index < midi_devs; dev_index++)
	{
	
		MIDIDeviceRef deviceRef = MIDIDeviceListGetDevice(devList, dev_index);
		MIDISetupRemoveDevice(deviceRef);
		MIDIObjectSetIntegerProperty(deviceRef, kMIDIPropertyOffline, true);

	}
	
	
	
	
	mHIDManagerRef = IOHIDManagerCreate(
													kCFAllocatorDefault, kIOHIDOptionsTypeNone);
	
	IOHIDManagerRegisterDeviceMatchingCallback(
											   mHIDManagerRef, CTProdikeysHIDMidiDriver::match_callback, (void *)this);
	
	IOHIDManagerScheduleWithRunLoop(
									mHIDManagerRef, MIDIGetDriverIORunLoop(), kCFRunLoopCommonModes);
	
	CFDictionaryRef match = matching_dictionary_create(kMyVendorID, kMyProductID, 0xFF00, 1);

	IOHIDManagerSetDeviceMatching(mHIDManagerRef, match);
	CFRelease(match);
	
	return noErr;

}

OSStatus		CTProdikeysHIDMidiDriver::Stop()
{

	IOHIDManagerUnscheduleFromRunLoop(mHIDManagerRef, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
	IOHIDDeviceClose(mHIDDevice->GetDeviceRef(), 0);
	return noErr;
	
}



//--------------------------------------------------------

void CTProdikeysHIDMidiDriver::got_hid_report(void *context, IOReturn result, void *sender,
					IOHIDReportType type, uint32_t reportID, uint8_t *report,
					CFIndex reportLength)
{
	
	CTProdikeysHIDMidiDriver *self = (CTProdikeysHIDMidiDriver *)context;
	
	if (reportID == 0x3) {
		uint8_t last_index = 0;
		
		//Need to find out how many
		while (report[last_index]!=0x0 && last_index < 31)
		{
			last_index++;
		}
		
		uint8_t number_of_keys = last_index / 2;
		
		Byte *midi_buffer = (Byte *)malloc(sizeof(Byte)*number_of_keys*4);
		memset(midi_buffer, 0x0, number_of_keys*4);
		
		for (uint16_t note_index = 0; note_index < number_of_keys; note_index++)
		{
		
			const uint16_t index_from_report = ((note_index * 2) + 1);
			const uint16_t index_into_midi_buffer = (note_index * 4);
			const uint16_t octave_offset = ((self->mCurrentOctave+2)*12)-BOTTOM_F_DISTANCE_FROM_MIDDLE_C;
			
			uint8_t HI = report[index_from_report];
			uint8_t Velocity = report[index_from_report + 1];
			Byte Note = (HI&0x3F) + octave_offset;
			Byte Status = ((HI & 0xC0) == 0x40) ? 0x90  : 0x80;
			
			midi_buffer[index_into_midi_buffer + 0] = (Status >> 4); //This is following the USB MidiStreaming Class. 
			midi_buffer[index_into_midi_buffer + 1] = Status;
			midi_buffer[index_into_midi_buffer + 2] = Note;
			midi_buffer[index_into_midi_buffer + 3] = Velocity;
			
		}
		
		self->HandleInput( self->mProdikeysDevice, mach_absolute_time(), midi_buffer, number_of_keys*4);
		
		free(midi_buffer);
		

		
		
		
	}
	else if (reportID == 0x4)
	{
		if ((report[1] & 0x10) == 0x10)
		{
			self->mCurrentOctave += 1;
			if (self->mCurrentOctave > 5)
			{
				self->mCurrentOctave = 5;
			}
		}
		
		if ((report[3] & 0x10) == 0x10)
		{
			self->mCurrentOctave -= 1;
			if (self->mCurrentOctave < -1)
			{
				self->mCurrentOctave = -1;
			}

		}
		
		if ((report[3] & 0x20) == 0x20)
		{
			self->sustainPedalDown = !self->sustainPedalDown;
			uint8_t midi_buffer[4] = {0xB, 0xB0, 0x40, 0x00};
			
			midi_buffer[3] = self->sustainPedalDown ? 0x7F : 0x00;
			self->HandleInput( self->mProdikeysDevice, mach_absolute_time(), midi_buffer, 4);

		}
		
	}
	else {
		
	}
	
}


void CTProdikeysHIDMidiDriver::match_set(CFMutableDictionaryRef dict, CFStringRef key, int value) {
	CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &value);
	CFDictionarySetValue(dict, key, number);
	CFRelease(number);
}

void CTProdikeysHIDMidiDriver::match_callback(void *context, IOReturn result,
										 void *sender, IOHIDDeviceRef device)
{
	
	
	
	IOReturn r = IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone); //May have to do seize, but that will disable all the cool consumer controls.
	if (r == kIOReturnSuccess) {
		CTProdikeysHIDMidiDriver *self = (CTProdikeysHIDMidiDriver *)context;
		self->mReportBuffer = new IOBuffer();
		self->mReportBuffer->Allocate(self, 120);
		
		self->mHIDDevice = new HIDDevice(device);

		IOHIDDeviceRegisterInputReportCallback(
											   device,
											   self->mReportBuffer->Buffer(),
											   self->mReportBuffer->Size(),
											   &CTProdikeysHIDMidiDriver::got_hid_report,
											   (void*)self);
		
		MIDIDeviceRef deviceRef = self->CreateDevice(self->mHIDDevice);
		
		self->mProdikeysDevice = self->CreateHIDMIDIDevice(self->mHIDDevice, deviceRef);
		self->mProdikeysDevice->Initialize();
		MIDIObjectSetIntegerProperty(deviceRef, kMIDIPropertyOffline, false);
		
		
		
		MIDISetupAddDevice(deviceRef);
		
	}
	else {
		
	}
}


CFDictionaryRef  CTProdikeysHIDMidiDriver::matching_dictionary_create(int vendorID,
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


bool		CTProdikeysHIDMidiDriver::MatchDevice(	HIDDevice *		inUSBDevice)
{
	return true;
}

MIDIDeviceRef	CTProdikeysHIDMidiDriver::CreateDevice(	HIDDevice *		inUSBDevice)
{

	

	MIDIDeviceRef dev;
	MIDIEntityRef ent;
	//UInt16 devProduct = USBToHostWord(inUSBDevice->GetDeviceDescriptor()->idProduct);
	
	
	//Setup HID Keyboard stuff Here...
	
	CFStringRef boxName = CFSTR("Prodikeys PC-MIDI");
	MIDIDeviceCreate(Self(),
                     boxName,
                     CFSTR("Creative Technology"),	// manufacturer name
                     boxName,
                     &dev);
	
	// make entity with 1 source
	for (int port = 1; port <= kMyNumPorts; ++port) {
		char portname[64];
		if (kMyNumPorts > 1)
			sprintf(portname, "Port %d", port);
		else
			CFStringGetCString(boxName, portname, sizeof(portname), kCFStringEncodingMacRoman);
        
		CFStringRef str = CFStringCreateWithCString(NULL, portname, 0);
		MIDIDeviceAddEntity(dev, str, true, 1, 1, &ent);
		CFRelease(str);
		MIDIObjectSetIntegerProperty(dev, kMIDIPropertyOffline, false);

	}
    
	return dev;
}

void		CTProdikeysHIDMidiDriver::StartInterface(HIDMIDIDevice *usbmDev)
{
	
	
	
}

void		CTProdikeysHIDMidiDriver::StopInterface(HIDMIDIDevice *usbmDev)
{
}

void		CTProdikeysHIDMidiDriver::HandleInput(HIDMIDIDevice *usbmDev, MIDITimeStamp when, Byte *readBuf, ByteCount readBufSize)
{
	
	HIDMIDIHandleInput(usbmDev, when, readBuf, readBufSize);
}

ByteCount	CTProdikeysHIDMidiDriver::PrepareOutput(HIDMIDIDevice *usbmDev, WriteQueue &writeQueue,
                                              Byte *destBuf)
{
	return 0;
}

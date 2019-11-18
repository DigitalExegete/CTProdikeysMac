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
#ifndef __GenericUSBMidi_h__
#define __GenericUSBMidi_h__

#include "HIDVendorMIDIDriver.h"
#include "MIDIDriverUtils.h"

class HIDDevice;

class CTProdikeysHIDMidiDriver : public HIDVendorMIDIDriver {
public:
	CTProdikeysHIDMidiDriver();
	~CTProdikeysHIDMidiDriver();
	
	
	static void got_hid_report(void *context, IOReturn result, void *sender,
							   IOHIDReportType type, uint32_t reportID, uint8_t *report,
							   CFIndex reportLength);

	void match_set(CFMutableDictionaryRef dict, CFStringRef key, int value);
	
	CFDictionaryRef matching_dictionary_create(int vendorID,
											   int productID,
											   int usagePage,
											   int usage);
	
	static void match_callback(void *context, IOReturn result,
							   void *sender, IOHIDDeviceRef device);

	
	virtual OSStatus		Start(MIDIDeviceListRef devList) override;
	virtual OSStatus		Stop() override;

	
	// USBMIDIDriverBase overrides
	virtual bool			MatchDevice(		HIDDevice *		inUSBDevice) override;
    
	virtual MIDIDeviceRef	CreateDevice(		HIDDevice *		inUSBDevice) override;
	
	virtual void		StartInterface(			HIDMIDIDevice *	usbmDev) override;
    // pipes are opened, do any extra initialization (send config msgs etc)
    
	virtual void		StopInterface(			HIDMIDIDevice *	usbmDev) override;
    // pipes are about to be closed, do any preliminary cleanup
    
	virtual void		HandleInput(			HIDMIDIDevice *	usbmDev,
                                    MIDITimeStamp	when,
                                    Byte *			readBuf,
                                    ByteCount 		readBufSize) override;
    // a USB message arrived, parse it into a MIDIPacketList and
    // call MIDIReceived
    
	virtual ByteCount	PrepareOutput(			HIDMIDIDevice *	usbmDev,
                                      WriteQueue &	writeQueue,
                                      Byte *			destBuf) override;
    // dequeue from WriteQueue into a single USB message, return

	IOBuffer * mReportBuffer;
	IOHIDManagerRef mHIDManagerRef;
	HIDDevice * mHIDDevice;
	HIDMIDIDevice * mProdikeysDevice;
	int8_t mCurrentOctave;
	bool sustainPedalDown;
	
};

#endif // __GenericUSBMidi_h__

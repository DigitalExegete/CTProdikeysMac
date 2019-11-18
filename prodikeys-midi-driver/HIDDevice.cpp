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
#include "HIDDevice.h"
#include <IOKit/IOCFPlugIn.h>
#include <unistd.h>

#ifndef kIOUSBDeviceInterfaceID245
#define kIOUSBDeviceInterfaceID245 CFUUIDGetConstantUUIDWithBytes(NULL, \
   0xFE, 0x2F, 0xD5, 0x2F, 0x3B, 0x5A, 0x47, 0x3B, 			\
   0x97, 0x7B, 0xAD, 0x99, 0x00, 0x1E, 0xB3, 0xED)
#endif

#ifndef kIOUSBInterfaceInterfaceID245
#define kIOUSBInterfaceInterfaceID245 CFUUIDGetConstantUUIDWithBytes(NULL, 	\
	0x64, 0xBA, 0xBD, 0xD2, 0x0F, 0x6B, 0x4B, 0x4F, 				\
	0x8E, 0x3E, 0xDC, 0x36, 0x04, 0x69, 0x87, 0xAD)
#endif

#if DEBUG
//#define VERBOSE 1
#endif

// _________________________________________________________________________________________
//	USBDevice::USBDevice
//
HIDDevice::HIDDevice(IOHIDDeviceRef ioDeviceObj) :
	mIOService(ioDeviceObj),
	mPluginIntf(NULL),
	mLocationID(0),
	mLangID(0),
	mNumLangIDs(0),
	mLangIDs(NULL),
	mReportBuffer(NULL)
{
	CFRetain(ioDeviceObj);
}

// _________________________________________________________________________________________
//	USBDevice::~USBDevice
//
HIDDevice::~HIDDevice()
{
	if (mPluginIntf != NULL)
		(*mPluginIntf)->Release(mPluginIntf);
	
	CFRelease(mIOService);
	
	if (mReportBuffer != NULL)
		delete mReportBuffer;
	
	delete[] mLangIDs;
}

// _________________________________________________________________________________________
//	USBDevice::GetPluginInterface
//
IOHIDDeviceDeviceInterface **	HIDDevice::GetPluginInterface()
{
	if (mPluginIntf == NULL) {
		// Get plugin interface to device
		IOCFPlugInInterface 	**ioPlugin;
		IOReturn	 			kr;
		SInt32 					score;

		io_service_t			hid_service_t = IOHIDDeviceGetService(mIOService);
		
		kr = IOCreatePlugInInterfaceForService(
			hid_service_t, kIOHIDDeviceTypeID, kIOCFPlugInInterfaceID,
			&ioPlugin, &score);

		if (kr) {
			usleep(100 * 1000);	// wait 100 ms
	
			IOCreatePlugInInterfaceForService(
				hid_service_t, kIOHIDDeviceTypeID, kIOCFPlugInInterfaceID,
				&ioPlugin, &score);
		}
		kr = (*ioPlugin)->QueryInterface(ioPlugin, CFUUIDGetUUIDBytes(kIOHIDDeviceDeviceInterfaceID2), (LPVOID *)&mPluginIntf);
		(*ioPlugin)->Release(ioPlugin);
		ioPlugin = NULL;
		if (kr) {
			__Debug_String("QueryInterface failed");
			mPluginIntf = NULL;
		} else {
#if VERBOSE
			printf("device @ 0x%08lX\n", mLocationID);
#endif
		}
	}
errexit:
	return mPluginIntf;
}


// _________________________________________________________________________________________
//	USBDevice::GetDescriptor
//


// _________________________________________________________________________________________
//	USBDevice::OpenAndConfigure
//
bool		HIDDevice::OpenAndConfigure(UInt8 configIndex)
{

	
	
//	mReportBuffer = new IOBuffer();
//	mReportBuffer->Allocate((void *)mReportBuffer, 120);
//
//
//	IOReturn r = IOHIDDeviceOpen(mIOService, kIOHIDOptionsTypeNone);
//	if (r == kIOReturnSuccess) {
//
//		IOHIDDeviceRegisterInputReportCallback(
//											   mIOService,
//											   mReportBuffer->Buffer(),
//											   mReportBuffer->Size(),
//											   &HIDDevice::got_hid_report,
//											   (void*) mReportBuffer);
//	}
//	else {
//
//	}

	
	return true;
}




// _________________________________________________________________________________________
//	USBDevice::FindInterface
//
//USBInterface *	HIDDevice::FindInterface(
//											UInt8					desiredInterfaceNumber,
//											UInt8					desiredAlternateSetting)
//{
//	IOUSBDeviceInterface **deviceIntf = GetPluginInterface();
//	if (deviceIntf == NULL)
//		return NULL;
//
//	// Create the interface iterator
//	IOUSBFindInterfaceRequest 	intfRequest;
//	io_service_t				ioInterfaceObj;
//	io_iterator_t				intfIter = 0;
//	intfRequest.bInterfaceClass		= kIOUSBFindInterfaceDontCare;
//	intfRequest.bInterfaceSubClass	= kIOUSBFindInterfaceDontCare;
//	intfRequest.bInterfaceProtocol	= kIOUSBFindInterfaceDontCare;
//	intfRequest.bAlternateSetting	= desiredAlternateSetting;
//
//	__Require_noErr((*deviceIntf)->CreateInterfaceIterator(deviceIntf, &intfRequest, &intfIter), errexit);
//
//	while ((ioInterfaceObj = IOIteratorNext(intfIter)) != 0) {
//		USBInterface *interface = new USBInterface(this, ioInterfaceObj);
//		IOUSBInterfaceInterface **intfIntf = interface->GetPluginInterface();
//		if (intfIntf != NULL) {
//			UInt8 intfNum;
//			__Require_noErr((*intfIntf)->GetInterfaceNumber(intfIntf, &intfNum), nextInterface);
//			if (intfNum == desiredInterfaceNumber) {
//				IOObjectRelease(ioInterfaceObj);
//				IOObjectRelease(intfIter);
//				return interface;
//			}
//		}
//nextInterface:
//		IOObjectRelease(ioInterfaceObj);
//		delete interface;
//	}
//	IOObjectRelease(intfIter);
//
//errexit:
//	return NULL;
//}

// _________________________________________________________________________________________
//	USBDevice::FindInterface
//
//USBInterface *	USBDevice::FindInterface(	IOUSBFindInterfaceRequest &intfRequest)
//{
//	IOUSBDeviceInterface **deviceIntf = GetPluginInterface();
//	if (deviceIntf == NULL)
//		return NULL;
//
//	// Create the interface iterator
//	io_service_t				ioInterfaceObj;
//	io_iterator_t				intfIter = 0;
//
//	__Require_noErr((*deviceIntf)->CreateInterfaceIterator(deviceIntf, &intfRequest, &intfIter), errexit);
//
//	if ((ioInterfaceObj = IOIteratorNext(intfIter)) != 0) {
//		USBInterface *interface = new USBInterface(this, ioInterfaceObj);
//		IOObjectRelease(ioInterfaceObj);
//		IOObjectRelease(intfIter);
//		return interface;
//	}
//	IOObjectRelease(intfIter);
//
//errexit:
//	return NULL;
//}
//

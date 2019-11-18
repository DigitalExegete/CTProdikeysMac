/* add your code here */
#include <IOKit/IOBufferMemoryDescriptor.h>
#include "CTProdikeysMac.hpp"

OSDefineMetaClassAndStructors(TMMTCTProdikeysMIDI, IOUSBHostHIDDevice)
#define super IOUSBHostHIDDevice


bool TMMTCTProdikeysMIDI::handleStart(IOService *provider)
{

	bool retValue = super::handleStart(provider);
	inMidiMode = false;
	fnKeyActive = false;
	sustainDown = false;
	
	OSString::withCString(_device->getLocation());
	
	return retValue;
	
}

IOReturn TMMTCTProdikeysMIDI::newReportDescriptor(IOMemoryDescriptor **desc) const {
	
	OSData *reportDescriptor = OSDynamicCast(OSData, getProperty(REPORT_DESCRIPTOR_OVERRIDE_KEY));
	
	if(reportDescriptor) {
		IOBufferMemoryDescriptor *bufferDesc = IOBufferMemoryDescriptor::withBytes(reportDescriptor->getBytesNoCopy(),
																				   reportDescriptor->getLength(),
																				   kIODirectionOutIn);
		
		
		
		if(bufferDesc) {
			*desc = bufferDesc;
			return kIOReturnSuccess;
		} else {
			bufferDesc->release();
			*desc = NULL;
			return kIOReturnNoMemory;
		}
		

		
	} else {
//		IOLog("IOUSBHIDDriverDescriptorOverride(%s)[%p]::newReportDescriptor - "
//			  "No %s data in personality, calling IOUSBHIDDriver::newReportDescriptor\n",
//			  getName(), this, REPORT_DESCRIPTOR_OVERRIDE_KEY);
		return IOUSBHostHIDDevice::newReportDescriptor(desc);
	}
}


IOReturn TMMTCTProdikeysMIDI::handleReport(
					  IOMemoryDescriptor * report,
					  IOHIDReportType      reportType,
					  IOOptionBits         options )
{
	
	
	
	uint8_t input_report_id = 0;
	
	report->readBytes(0, &input_report_id, 1);
	
	
	
	if ((input_report_id == 1) && inMidiMode)
	{
		uint8_t consumer_control_report_buffer[4] = {0x0};
		uint8_t midi_control_report_buffer[4] = { 0x4, 0x0, 0x0, 0x10 };
		uint8_t midi_sustain_control_report_buffer[4] = { 0x4, 0x0, 0x0, 0x20 };
		report->readBytes(0, consumer_control_report_buffer, report->getLength()); //should be 4
		
		if ((consumer_control_report_buffer[2] & 0x40) == 0x40) //interpret as octave down key.
		{
			
			IOBufferMemoryDescriptor *midi_descriptor = IOBufferMemoryDescriptor::withBytes(midi_control_report_buffer, 4, kIODirectionOutIn);
			IOReturn retVal = super::handleReport(midi_descriptor, reportType, options);
			midi_descriptor->release();
			
			return retVal;
			
		}
		else if ((consumer_control_report_buffer[3] & 0x4) == 0x04) //Sustain Button for now, push on, push off.
		{
			
			IOBufferMemoryDescriptor *midi_descriptor = IOBufferMemoryDescriptor::withBytes(midi_sustain_control_report_buffer, 4, kIODirectionOutIn);
			IOReturn retVal = super::handleReport(midi_descriptor, reportType, options);
			midi_descriptor->release();
			
			return retVal;

		}
		else
		{
			super::handleReport(report, reportType, options);
		}
		
		
	}
	if (input_report_id == 3)
	{
		uint8_t prodikeys_buffer[31] = {0xFF};
		prodikeys_buffer[0] = 0x03;
		
		IOByteCount rLength = report->getLength();
		
		report->readBytes(0, prodikeys_buffer, rLength);
		
		IOBufferMemoryDescriptor *replacementReport = IOBufferMemoryDescriptor::withBytes(prodikeys_buffer, 31, kIODirectionOutIn);
		
		IOReturn retVal = super::handleReport(replacementReport, reportType, options);
		
		replacementReport->release();
		
		return retVal;
		
	}
	if (input_report_id == 4)
	{
		
		uint8_t report_buffer[4];
		report->readBytes(0, report_buffer, 4);
		
		if ((report_buffer[1] & 0x2) == 0x2) //MIDI Button was pressed -- send up the on/off status!
		{
			
			//04 02 00 10 ==> MIDI is on
			//04 02 00 00 ==> MIDI is off.
			
			inMidiMode  = !inMidiMode;
			
			uint8_t report_data[3] = {0x6, 0x1, 0xc1};
			
			if (!inMidiMode)
			{
				report_data[2] = 0xc2;
			}
			
			IOBufferMemoryDescriptor *memDesc = IOBufferMemoryDescriptor::withBytes(report_data, 3, kIODirectionOutIn);
			
			setReport(memDesc, kIOHIDReportTypeOutput, 0x6);
			
			memDesc->release();
			
			
			
			setProperty("isMidiActive", inMidiMode);
//			setProperty("usbLocationID", );
			//IOReturn retVal = IOHIDDeviceSetReport(handler->device, kIOHIDReportTypeOutput, 6, report_data, 3);
			if (inMidiMode)
			{
				report_buffer[3] |= 0x10;
				IOBufferMemoryDescriptor *alternateDescriptor = IOBufferMemoryDescriptor::withBytes(report_buffer, 4, kIODirectionOutIn);
				IOReturn returnValue = super::handleReport(alternateDescriptor);
				alternateDescriptor->release();
				return returnValue;
			}
			

			
		}
		
		if ((report_buffer[3] & 0x10) == 0x10) //FN Button was pressed
		{
			
			fnKeyActive  = !fnKeyActive;
			
			uint8_t report_data[3] = {0x6, 0x1, 0xc5};
			
			if (!fnKeyActive)
			{
				report_data[2] = 0xc6;
			}
			
			IOBufferMemoryDescriptor *memDesc = IOBufferMemoryDescriptor::withBytes(report_data, 3, kIODirectionOutIn);
			
			setReport(memDesc, kIOHIDReportTypeOutput, 0x6);
			
			memDesc->release();
			
			//IOReturn retVal = IOHIDDeviceSetReport(handler->device, kIOHIDReportTypeOutput, 6, report_data, 3);
			
			
		}
		
		return super::handleReport(report, reportType, options);

	}
	else
	{
		return super::handleReport(report, reportType, options);
	}
	
}


void TMMTCTProdikeysMIDI::interruptReadComplete (void *parameter, IOReturn status, uint32_t bytesTransferred)
{
	
	super::interruptReadComplete(parameter, status, bytesTransferred);
	
}

/*
 
 No Kext:
 
 | |   |     +-o Creative Prodikeys PC-MIDI@3a200000  <class IORegistryEntry:IOService:IOUSBNub:IOUSBDevice, id 0x100002949, registered, matched, active, busy 0 (660 ms), retain 30>
 | |   |       +-o AppleUSBHostLegacyClient  <class IORegistryEntry:IOService:AppleUSBHostLegacyClient, id 0x10000294c, !registered, !matched, active, busy 0, retain 9>
 | |   |       +-o AppleUSBHostCompositeDevice  <class IORegistryEntry:IOService:AppleUSBHostCompositeDevice, id 0x100002955, !registered, !matched, active, busy 0, retain 4>
 | |   |       +-o IOUSBHostInterface@0  <class IORegistryEntry:IOService:IOUSBNub:IOUSBInterface, id 0x100002957, registered, matched, active, busy 0 (9 ms), retain 11>
 | |   |       | +-o IOUSBHostHIDDevice@3a200000,0  <class IORegistryEntry:IOService:IOHIDDevice:IOUSBHostHIDDevice, id 0x10000295d, registered, matched, active, busy 0 (3 ms), retain 10>
 | |   |       |   +-o IOHIDInterface  <class IORegistryEntry:IOService:IOHIDInterface, id 0x100002964, registered, matched, active, busy 0 (3 ms), retain 7>
 | |   |       |   | +-o IOHIDEventDriver  <class IORegistryEntry:IOService:IOHIDEventService:IOHIDEventDriver, id 0x100002966, registered, matched, active, busy 0 (0 ms), retain 8>
 | |   |       |   |   +-o IOHIDEventServiceUserClient  <class IORegistryEntry:IOService:IOUserClient:IOHIDEventServiceUserClient, id 0x100002968, !registered, !matched, active, busy 0, retain 6>
 | |   |       |   +-o IOHIDLibUserClient  <class IORegistryEntry:IOService:IOUserClient:IOHIDLibUserClient, id 0x100002965, !registered, !matched, active, busy 0, retain 6>
 | |   |       |   +-o IOHIDLibUserClient  <class IORegistryEntry:IOService:IOUserClient:IOHIDLibUserClient, id 0x10000297a, !registered, !matched, active, busy 0, retain 6>
 | |   |       +-o IOUSBHostInterface@1  <class IORegistryEntry:IOService:IOUSBNub:IOUSBInterface, id 0x100002958, registered, matched, active, busy 0 (11 ms), retain 12>
 | |   |         +-o IOUSBHostHIDDevice@3a200000,1  <class IORegistryEntry:IOService:IOHIDDevice:IOUSBHostHIDDevice, id 0x100002961, registered, matched, active, busy 0 (5 ms), retain 10>
 | |   |           +-o IOHIDInterface  <class IORegistryEntry:IOService:IOHIDInterface, id 0x10000296b, registered, matched, active, busy 0 (4 ms), retain 7>
 | |   |           | +-o IOHIDEventDriver  <class IORegistryEntry:IOService:IOHIDEventService:IOHIDEventDriver, id 0x10000296d, registered, matched, active, busy 0 (0 ms), retain 8>
 | |   |           |   +-o IOHIDEventServiceUserClient  <class IORegistryEntry:IOService:IOUserClient:IOHIDEventServiceUserClient, id 0x10000296f, !registered, !matched, active, busy 0, retain 6>
 | |   |           +-o IOHIDLibUserClient  <class IORegistryEntry:IOService:IOUserClient:IOHIDLibUserClient, id 0x10000296c, !registered, !matched, active, busy 0, retain 6>
 | |   |           +-o IOHIDLibUserClient  <class IORegistryEntry:IOService:IOUserClient:IOHIDLibUserClient, id 0x100002979, !registered, !matched, active, busy 0, retain 6>

 with my kext:
 
 | |   |     +-o Creative Prodikeys PC-MIDI@3a200000  <class IORegistryEntry:IOService:IOUSBNub:IOUSBDevice, id 0x1000029c7, registered, matched, active, busy 0 (118 ms), retain 30>
 | |   |       +-o AppleUSBHostLegacyClient  <class IORegistryEntry:IOService:AppleUSBHostLegacyClient, id 0x1000029ca, !registered, !matched, active, busy 0, retain 9>
 | |   |       +-o AppleUSBHostCompositeDevice  <class IORegistryEntry:IOService:AppleUSBHostCompositeDevice, id 0x1000029d3, !registered, !matched, active, busy 0, retain 4>
 | |   |       +-o IOUSBHostInterface@0  <class IORegistryEntry:IOService:IOUSBNub:IOUSBInterface, id 0x1000029d5, registered, matched, active, busy 0 (113 ms), retain 11>
 | |   |       | +-o IOUSBHostHIDDevice@3a200000,0  <class IORegistryEntry:IOService:IOHIDDevice:IOUSBHostHIDDevice, id 0x1000029dc, registered, matched, active, busy 0 (108 ms), retain 9>
 | |   |       |   +-o IOHIDInterface  <class IORegistryEntry:IOService:IOHIDInterface, id 0x1000029e6, registered, matched, active, busy 0 (107 ms), retain 7>
 | |   |       |   | +-o IOHIDEventDriver  <class IORegistryEntry:IOService:IOHIDEventService:IOHIDEventDriver, id 0x1000029e9, registered, matched, active, busy 0 (1 ms), retain 8>
 | |   |       |   |   +-o IOHIDEventServiceUserClient  <class IORegistryEntry:IOService:IOUserClient:IOHIDEventServiceUserClient, id 0x1000029eb, !registered, !matched, active, busy 0, retain 6>
 | |   |       |   +-o IOHIDLibUserClient  <class IORegistryEntry:IOService:IOUserClient:IOHIDLibUserClient, id 0x1000029e8, !registered, !matched, active, busy 0, retain 6>
 | |   |       +-o IOUSBHostInterface@1  <class IORegistryEntry:IOService:IOUSBNub:IOUSBInterface, id 0x1000029d6, registered, matched, active, busy 0 (10 ms), retain 11>
 | |   |         +-o TMMTCTProdikeysMIDI@3a200000,1  <class IORegistryEntry:IOService:IOHIDDevice:IOUSBHostHIDDevice:TMMTCTProdikeysMIDI, id 0x1000029df, registered, matched, active, busy 0 (7 ms), retain 9>
 | |   |           +-o IOHIDInterface  <class IORegistryEntry:IOService:IOHIDInterface, id 0x1000029e7, registered, matched, active, busy 0 (6 ms), retain 6>
 | |   |           +-o IOHIDLibUserClient  <class IORegistryEntry:IOService:IOUserClient:IOHIDLibUserClient, id 0x1000029e5, !registered, !matched, active, busy 0, retain 6>

 
 
 
 */


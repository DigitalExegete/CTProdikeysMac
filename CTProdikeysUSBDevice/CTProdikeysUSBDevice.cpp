/* add your code here */
#include "CTProdikeysUSBDevice.hpp"
OSDefineMetaClassAndStructors(TMMTCTProdikeysUSB, IOUSBHostHIDDevice)

const StandardUSB::DeviceDescriptor* TMMTCTProdikeysUSB::getDeviceDescriptor()
{
	
	StandardUSB::DeviceDescriptor *descriptor = new StandardUSB::DeviceDescriptor();
	
	descriptor->bLength = 0x12;
	descriptor->bDescriptorType = 0x1;
	
	descriptor->bcdUSB = 0x0200;
	descriptor->bDeviceClass = 0x0;
	descriptor->bDeviceSubClass = 0x0;
	descriptor->bDeviceProtocol = 0x0;
	descriptor->bMaxPacketSize0 = 0x8;
	descriptor->idVendor = 0x041E;
	descriptor->idProduct = 0x2801;
	descriptor->bcdDevice = 0x0005;
	descriptor->iManufacturer = 0x1;
	descriptor->iProduct = 0x2;
	descriptor->iSerialNumber = 0x0;
	descriptor->bNumConfigurations = 0x1;
	
	return descriptor;
	
}

const StandardUSB::ConfigurationDescriptor* getConfigurationDescriptor(uint8_t index)
{
	
	StandardUSB::ConfigurationDescriptor* config_descriptor = new StandardUSB::ConfigurationDescriptor();
	
	config_descriptor->bLength = 0x9;
	config_descriptor->bDescriptorType = 0x2;
	
	/*
	 
	 09h
	 1	bDescriptorType	1	02h	Configuration
	 2	wTotalLength	2	0042h
	 4	bNumInterfaces	1	02h
	 5	bConfigurationValue	1	01h
	 6	iConfiguration	1	00h
	 7	bmAttributes	1	A0h
	 
	 */
	
	return config_descriptor;
	
}



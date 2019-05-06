/* add your code here */
#include <IOKit/usb/IOUSBHostHIDDevice.h>

#define REPORT_DESCRIPTOR_OVERRIDE_KEY "ReportDescriptorOverride"

class TMMTCTProdikeysMIDI : public IOUSBHostHIDDevice {
	
	OSDeclareDefaultStructors(TMMTCTProdikeysMIDI)
	
public:
	virtual IOReturn newReportDescriptor(IOMemoryDescriptor **descriptor) const override;
	
//	virtual bool start (IOService *provider) override;

	
	virtual void interruptReadComplete (void *parameter, IOReturn status, uint32_t bytesTransferred) override;
	//virtual IOReturn  readInterruptPipeAsync (void) override;

	virtual IOReturn handleReport(
								  IOMemoryDescriptor * report,
								  IOHIDReportType      reportType = kIOHIDReportTypeInput,
								  IOOptionBits         options    = 0 ) override;
	
	virtual bool handleStart( IOService * provider ) override;
	
private:
	bool inMidiMode;
	bool fnKeyActive;
	bool sustainDown;
	OSString *locationID;

	
};

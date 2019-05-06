/* add your code here */
#include <IOKit/usb/IOUSBHostDevice.h>


class TMMTCTProdikeysUSB : public IOUSBHostDevice
{
	OSDeclareDefaultStructors(TMMTCTProdikeysUSB)
	
	/*!
	* @brief       Return the device descriptor
	* @discussion  This method uses getDescriptor to retrieve the device descriptor.
	* @return      Pointer to the device descriptor, or NULL if the service is terminating.
		*/
		virtual const StandardUSB::DeviceDescriptor* getDeviceDescriptor() override;
		
	/*!
	 * @brief       Return the configuration descriptor with the specified index
	 * @discussion  This method uses getDescriptor to retrieve a configuration descriptor.
	 * @param       index Descriptor index value
	 * @return      Pointer to the configuration descriptor if found
	 */
		virtual const StandardUSB::ConfigurationDescriptor* getConfigurationDescriptor(uint8_t index) override;
		
	/*!
	 * @brief       Return the configuration descriptor with the specified <code>bConfigurationValue</code>
	 * @discussion  This method uses getDescriptor to search for a configuration descriptor with a specific <code>bConfigurationValue</code> field.
	 * @param       bConfigurationValue Value to match
	 * @return      Pointer to the configuration descriptor if found
	 */
		virtual const StandardUSB::ConfigurationDescriptor* getConfigurationDescriptorWithValue(uint8_t bConfigurationValue) override;
		
	/*!
	 * @brief       Return the currently selected configuration descriptor
	 * @discussion  This method uses getDescriptor to return the configuration descriptor currently selected after a successful setConfiguration call
	 * @return      Pointer to the configuration descriptor if found, or NULL if the device is not configured
	 */
		virtual const StandardUSB::ConfigurationDescriptor* getConfigurationDescriptor() override;

};


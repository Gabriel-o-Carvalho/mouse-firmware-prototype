/*
 * usb_descriptors.h
 *
 *  Created on: Apr 30, 2025
 *      Author: gabriel
 */

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#include <stdint.h>
#include "usb_standards.h"
#include "Hid/usb_hid_standards.h"

const UsbDeviceDescriptor device_descriptor = {
	.bLength = sizeof(UsbDeviceDescriptor),
	.bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = USB_CLASS_PER_INTERFACE,
	.bDeviceSubClass = 0x00,
	.bDeviceProtocol = 0x00,
	.bMaxPacketSize0 = 8,
	.idVendor = 0x6666,
	.idProduct = 0x1001,
	.bcdDevice = 0x0100,
	.iManufacturer = 0x00,
	.iProduct = 0x00,
	.iSerialNumber = 0x00,
	.bNumConfigurations = 1
};

typedef struct {
	UsbConfigurationDescriptor usb_config_descriptor;
	UsbInterfaceDescriptor usb_interface_descriptor;
	UsbHidDescriptor usb_hid_descriptor;
	UsbEndpointDescriptor usb_endpoint_descriptor;
} __attribute__((__packed__)) UsbConfigCombination;

typedef struct {
	int8_t x;
	int8_t y;
	uint8_t buttons;
} __attribute__((__packed__)) HidReport;

const uint8_t hid_report_descriptor[] = {
	HID_USAGE_PAGE(HID_PAGE_DESKTOP),
	HID_USAGE(HID_DESKTOP_MOUSE),
	HID_COLLECTION(HID_APPLICATION_COLLECTION),
		HID_USAGE(HID_DESKTOP_POINTER),
		HID_COLLECTION(HID_PHYSICAL_COLLECTION),
			// Pointer
			HID_USAGE(HID_DESKTOP_X),
			HID_USAGE(HID_DESKTOP_Y),
			HID_LOGICAL_MINIMUM(-127),
			HID_LOGICAL_MAXIMUM(127),
			HID_REPORT_SIZE(8),
			HID_REPORT_COUNT(2),
			HID_INPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

			// Mouse buttons
			HID_USAGE_PAGE(HID_PAGE_BUTTON),
			HID_USAGE_MINIMUM(1),
			HID_USAGE_MAXIMUM(3),
			HID_LOGICAL_MINIMUM(0),
			HID_LOGICAL_MAXIMUM(1),
			HID_REPORT_SIZE(1),
			HID_REPORT_COUNT(3),
			HID_INPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
			// Padding bits
			HID_REPORT_SIZE(1),
			HID_REPORT_COUNT(5),
			HID_INPUT(HID_IOF_CONSTANT),
		HID_END_COLLECTION,
	HID_END_COLLECTION,
};



const UsbConfigCombination config_combination_descriptor = {
	.usb_config_descriptor = {
		.bLength = sizeof(UsbConfigurationDescriptor),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION,
		.wTotalLength = sizeof(UsbConfigCombination),
		.bNumInterfaces = 1,
		.bConfigurationValue = 1,
		.iConfiguration = 0,
		.bmAttributes = 0x80 | 0x40,
		.bMaxPower = 25
	},
	.usb_interface_descriptor ={
		.bLength = sizeof(UsbInterfaceDescriptor),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
		.bInterfaceNumber = 1,
		.bAlternateSetting = 0,
		.bNumEndpoints = 1,
		.bInterfaceClass = USB_CLASS_HID,
		.bInterfaceSubClass = 0x00,
		.bInterfaceProtocol = 0x00,
		.iInterface = 0x00
	},

	.usb_hid_descriptor = {
		.bLength = sizeof(UsbHidDescriptor),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_HID,
		.bcdHID = 0x0100,
		.bCountryCode = USB_HID_COUNTRY_LATIN_AMERICA,
		.bNumDescriptors = 1,
		.bDescriptorType0 = USB_DESCRIPTOR_TYPE_HID_REPORT,
		.wDescriptorLength0 = sizeof(hid_report_descriptor)
	},
	.usb_endpoint_descriptor = {
		.bLength = sizeof(UsbEndpointDescriptor),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x81,
		.bmAttributes = USB_ENDPOINT_TYPE_INTERRUPT,
		.wMaxPacketSize = 64,
		.bInterval = 50
	}

};




#endif /* USB_DESCRIPTORS_H_ */

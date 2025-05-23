#ifndef USB_HID_STANDARDS_H_
#define USB_HID_STANDARDS_H_

typedef struct {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t bcdHID;
	uint8_t  bCountryCode;
	uint8_t  bNumDescriptors;
	uint8_t  bDescriptorType0;
	uint16_t wDescriptorLength0;
} __attribute__((__packed__)) UsbHidDescriptor;

#define USB_HID_COUNTRY_LATIN_AMERICA 17

#endif /* USB_HID_STANDARDS_H_ */

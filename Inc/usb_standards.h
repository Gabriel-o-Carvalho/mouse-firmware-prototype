#ifndef USB_STANDARDS_H_
#define USB_STANDARDS_H_
#include <stdint.h>
#include "stm32f1xx.h"

typedef struct {
	void (*on_usb_reset_received)();
	void (*on_setup_data_received)(uint8_t endpoint_number, uint16_t bcnt);
	void (*on_out_data_received)(uint8_t endpoint_number, uint16_t bcnt);
	void (*on_in_transfer_completed)(uint8_t endpoint_number);
	void (*on_out_transfer_completed)(uint8_t endpoint_number);
	void (*on_usb_polled)();
	void (*on_err_transfer_received)();

} UsbEvents;

typedef enum {
	USB_DEVICE_STATE_ATTACHED,
	USB_DEVICE_STATE_CONFIGURED,
	USB_DEVICE_STATE_DEFAULT,
	USB_DEVICE_STATE_SUSPENDED,
	USB_DEVICE_STATE_ADDRESSED
} UsbDeviceState;

typedef enum {
	USB_CONTROL_STAGE_SETUP,
	USB_CONTROL_STAGE_DATA_OUT,
	USB_CONTROL_STAGE_DATA_IN,
	USB_CONTROL_STAGE_DATA_IN_IDLE,
	USB_CONTROL_STAGE_DATA_IN_ZERO,
	USB_CONTROL_STAGE_STATUS_OUT,
	USB_CONTROL_STAGE_STATUS_IN,
} UsbControlTransferStage;

typedef struct {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} UsbRequest;


#define REQUEST_TYPE_DIRECTION                (0x1<<7)
#define REQUEST_TYPE_DIRECTION_HOST_TO_DEVICE (0x0 << 7)
#define REQUEST_TYPE_DIRECTION_DEVICE_TO_HOST (0x1 << 7)

#define REQUEST_TYPE_TYPE          (0x3 << 5)
#define REQUEST_TYPE_TYPE_STANDARD (0x0 << 5)
#define REQUEST_TYPE_TYPE_CLASS    (0x1 << 5)
#define REQUEST_TYPE_TYPE_VENDOR   (0x2 << 5)

#define REQUEST_TYPE_RECIPIENT           (0xF << 0)
#define REQUEST_TYPE_RECIPIENT_DEVICE    (0x0 << 0)
#define REQUEST_TYPE_RECIPIENT_INTERFACE (0x1 << 0)
#define REQUEST_TYPE_RECIPIENT_ENDPOINT  (0x2 << 0)
#define REQUEST_TYPE_RECIPIENT_OTHER     (0x3 << 0)

typedef struct {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t bcdUSB;
	uint8_t  bDeviceClass;
	uint8_t  bDeviceSubClass;
	uint8_t  bDeviceProtocol;
	uint8_t  bMaxPacketSize0;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t  iManufacturer;
	uint8_t  iProduct;
	uint8_t  iSerialNumber;
	uint8_t  bNumConfigurations;
} __attribute__((__packed__)) UsbDeviceDescriptor;

typedef struct {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t wTotalLength;
	uint8_t  bNumInterfaces;
	uint8_t  bConfigurationValue;
	uint8_t  iConfiguration;
	uint8_t  bmAttributes;
	uint8_t  bMaxPower;
} __attribute__((__packed__)) UsbConfigurationDescriptor;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
} __attribute__((__packed__)) UsbInterfaceDescriptor;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
} __attribute__((__packed__)) UsbEndpointDescriptor;


#define USB_DESCRIPTOR_TYPE_DEVICE           0x01
#define USB_DESCRIPTOR_TYPE_CONFIGURATION    0x02
#define USB_DESCRIPTOR_TYPE_STRING           0x03
#define USB_DESCRIPTOR_TYPE_INTERFACE        0x04
#define USB_DESCRIPTOR_TYPE_ENDPOINT         0x05
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER 0x06
#define USB_DESCRIPTOR_TYPE_HID				 0x21
#define USB_DESCRIPTOR_TYPE_HID_REPORT		 0x22


#define USB_CLASS_PER_INTERFACE 0x00

#define GET_STATUS        0x00
#define CLEAR_FEATURE     0x01
#define SET_FEATURE       0x03
#define SET_ADDRESS       0x05
#define GET_DESCRIPTOR    0x06
#define SET_DESCRIPTOR    0x07
#define GET_CONFIGURATION 0x08
#define SET_CONFIGURATION 0x09
#define GET_INTERFACE     0x0A
#define SET_INTERFACE     0x0B
#define SYNCH_FRAME       0x0C

#define USB_CLASS_HID     0x03

#define USB_ENDPOINT_TYPE_CONTROL 		0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS   0x01
#define USB_ENDPOINT_TYPE_BULK    		0x02
#define USB_ENDPOINT_TYPE_INTERRUPT  	0x03



#endif /* USB_STANDARDS_H_ */

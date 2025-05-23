#include "usbd_framework.h"
#include "usbd_driver.h"
#include "usb_standards.h"
#include "usb_device.h"
#include "usb_descriptors.h"

#include "Hid/usb_hid.h"

static UsbDevice *usbd_handle;
static uint8_t flag_hid = 0;
static uint8_t data_in_zero = 0;

void initialize_usb(UsbDevice *usb_device){
	usbd_handle = usb_device;
	usb_driver.initialize_usb();
}

static void usb_reset_handler(){
	usbd_handle->in_data_size = 0;
	usbd_handle->out_data_size = 0;
	usbd_handle->config_value = 0;
	usbd_handle->device_state = USB_DEVICE_STATE_DEFAULT;
	usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_SETUP;

}

void usbd_configure(){
	usb_driver.config_endpoint1();
	usb_driver.write_packet(0x01, 0, 0);

}

static void process_standard_device_request(){
	UsbRequest const *request = (UsbRequest *) usbd_handle->ptr_out_buffer;
	uint16_t data_size = (request->wLength);
	switch(request->bRequest){
		case GET_DESCRIPTOR:
			uint8_t descriptor_type = (request->wValue >> 8);
			switch(descriptor_type){
				case  USB_DESCRIPTOR_TYPE_DEVICE:
					usbd_handle->ptr_in_buffer = &device_descriptor;
					usbd_handle->in_data_size = data_size;
					usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN;
					break;

				case USB_DESCRIPTOR_TYPE_CONFIGURATION:
					usbd_handle->ptr_in_buffer = &config_combination_descriptor;
					usbd_handle->in_data_size = data_size;
					usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN;
					break;
			}
			break;

		case SET_ADDRESS:
			uint16_t device_addr = request->wValue;
			usbd_handle->in_data_size = data_size;
			usb_driver.set_device_address(device_addr);
			usbd_handle->device_state = USB_DEVICE_STATE_ADDRESSED;
			usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_IN;
			break;
		case SET_CONFIGURATION:
			usbd_handle->config_value = request->wValue;
			usbd_configure();
			usbd_handle->device_state = USB_DEVICE_STATE_CONFIGURED;
			usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_IN;
			break;
		default:
			break;
	}

}

static void process_class_interface_request(){
	UsbRequest const *request = (UsbRequest *) usbd_handle->ptr_out_buffer;

	switch(request->bRequest){
		case USB_HID_SETIDLE:
			usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_IN;
			break;
	}
}

static void process_standard_interface_request(){
	UsbRequest const *request = (UsbRequest *) usbd_handle->ptr_out_buffer;

	switch(request->wValue >> 8){
		case USB_DESCRIPTOR_TYPE_HID_REPORT:
			flag_hid = 1;
			usbd_handle->ptr_in_buffer = &hid_report_descriptor;
			usbd_handle->in_data_size = sizeof(hid_report_descriptor);
			usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN;
			break;
	}
}

static void process_request(){
	UsbRequest const *request = (UsbRequest *) usbd_handle->ptr_out_buffer;

	switch(request->bmRequestType & (REQUEST_TYPE_TYPE | REQUEST_TYPE_RECIPIENT)){
		case REQUEST_TYPE_TYPE_STANDARD | REQUEST_TYPE_RECIPIENT_DEVICE:
			process_standard_device_request();
			break;
		case REQUEST_TYPE_TYPE_CLASS | REQUEST_TYPE_RECIPIENT_INTERFACE:
			process_class_interface_request();
			break;

		case REQUEST_TYPE_TYPE_STANDARD | REQUEST_TYPE_RECIPIENT_INTERFACE:
			process_standard_interface_request();
			break;
		default:
			break;

	}
}

static void setup_data_received_handler(uint8_t endpoint_number, uint16_t byte_count){
	usb_driver.read_packet(usbd_handle->ptr_out_buffer, byte_count);
	process_request();
}

static void process_control_stage_transfer(){
	switch(usbd_handle->control_transfer_stage){
		case USB_CONTROL_STAGE_SETUP:
			break;

		case USB_CONTROL_STAGE_DATA_IN:
			uint8_t data_size = (device_descriptor.bMaxPacketSize0 > usbd_handle->in_data_size) ? usbd_handle->in_data_size : device_descriptor.bMaxPacketSize0;

			usb_driver.write_packet(0, usbd_handle->ptr_in_buffer, data_size);
			usbd_handle->ptr_in_buffer += data_size;
			usbd_handle->in_data_size -= data_size;

			if(usbd_handle->in_data_size == 0 && data_size == device_descriptor.bMaxPacketSize0){
				usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN_ZERO;
			}

			break;

		case USB_CONTROL_STAGE_STATUS_IN:
			usbd_handle->in_data_size = 0;
			usb_driver.write_packet(0,0,0);
			break;

		case  USB_CONTROL_STAGE_DATA_IN_ZERO:
			usbd_handle->in_data_size = 0;
			usb_driver.write_packet(0,0,0);
			usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_OUT;
			break;

		default:

			break;
	}
}

static void usb_polled_handler(){
	process_control_stage_transfer();


}

static void write_hid_report(){
	HidReport hid_report = {
			.x = 5,
			.y = 5,
			.buttons = 0
	};

	usb_driver.write_packet(0x01, &hid_report, sizeof(hid_report));
}

static void in_transfer_completed_handler(uint8_t endpoint_number){
	if(usbd_handle->in_data_size > 0)
		usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN;

	if(usbd_handle->in_data_size == 0 && usbd_handle->control_transfer_stage != USB_CONTROL_STAGE_DATA_IN_ZERO){
		usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_OUT;
	}

	if(usbd_handle->control_transfer_stage == USB_CONTROL_STAGE_STATUS_IN){
		usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_SETUP;
	}

	if(endpoint_number == 0x01){
		write_hid_report();
	}
}

static void out_transfer_completed_handler(uint8_t endpoint_number){
	usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_SETUP;
}

static void err_transfer_received_handler(){
	usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_SETUP;
}

UsbEvents usb_events = {
	.on_usb_reset_received = &usb_reset_handler,
	.on_setup_data_received = &setup_data_received_handler,
	.on_usb_polled = &usb_polled_handler,
	.on_in_transfer_completed = &in_transfer_completed_handler,
	.on_out_transfer_completed = &out_transfer_completed_handler,
	.on_err_transfer_received = &err_transfer_received_handler
};







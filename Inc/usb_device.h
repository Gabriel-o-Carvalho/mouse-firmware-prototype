#ifndef USB_DEVICE_H_
#define USB_DEVICE_H_

typedef struct {
	UsbDeviceState device_state;

	UsbControlTransferStage control_transfer_stage;

	uint8_t config_value;

	void const  *ptr_out_buffer;
	uint32_t out_data_size;
	void const *ptr_in_buffer;
	uint32_t in_data_size;
} UsbDevice;


#endif /* USB_DEVICE_H_ */

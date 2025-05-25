/*
 * framework.h
 *
 *  Created on: Apr 26, 2025
 *      Author: gabriel
 */

#ifndef USBD_FRAMEWORK_H_
#define USBD_FRAMEWORK_H_

#include "usbd_driver.h"
#include "usb_device.h"

typedef struct {
	void (*set_mousex)(int8_t value);
	void (*set_mousey)(int8_t value);
	void (*set_buttons)(uint8_t buttons);
} HidConfig;

extern HidConfig hid_config;
void initialize_usb(UsbDevice *usb_device);

#endif /* USBD_FRAMEWORK_H_ */

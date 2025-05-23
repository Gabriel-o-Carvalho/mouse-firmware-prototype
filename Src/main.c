#include <stdio.h>

#include "stm32f1xx.h"

#include "usbd_framework.h"
#include "usb_device.h"

UsbDevice volatile usb_device;
uint8_t buffer[8];

int main(void){
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);

	MODIFY_REG(GPIOC->CRH,
			   GPIO_CRH_CNF13 | GPIO_CRH_MODE13,
			   GPIO_CRH_MODE13_0
	          );


	usb_device.ptr_out_buffer = buffer;

    initialize_usb(&usb_device);

    while(1){

	}
}

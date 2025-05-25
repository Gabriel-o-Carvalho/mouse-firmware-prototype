#include <stdio.h>
#include <stdint.h>

#include "stm32f1xx.h"
#include "adc_driver.h"
#include "usbd_framework.h"


volatile UsbDevice usb_device;

uint8_t buffer[8];
int8_t x_value, y_value;
uint8_t mouse_buttons;

float map(float value, float old_min, float old_max, float new_min, float new_max){
	return ( value - old_min) / (old_max - old_min) * ( new_max - new_min) + new_min;
}

int main(void){
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);

	usb_device.ptr_out_buffer = buffer;
    initialize_usb(&usb_device);

    adc_init();

    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |=   GPIO_CRH_MODE13_0;

    GPIOA->CRL &= ~(GPIO_CRL_MODE2 | GPIO_CRL_MODE3);
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_CNF3);
    GPIOA->CRL |=  (GPIO_CRL_CNF2_1 | GPIO_CRL_CNF3_1);

    CLEAR_BIT(GPIOA->ODR, GPIO_ODR_ODR2 | GPIO_ODR_ODR3);

    while(1){
    	x_value = (int8_t) map(get_ch_value(0), 0, 4095, -127, 127);
    	y_value = (int8_t) map(get_ch_value(1), 0, 4095, -127, 127);
    	uint8_t right_button = (GPIOA->IDR & GPIO_IDR_IDR2) >> GPIO_IDR_IDR2_Pos;
    	uint8_t left_button = (GPIOA->IDR & GPIO_IDR_IDR3) >> GPIO_IDR_IDR3_Pos;

    	if(right_button){
    		GPIOC->ODR |=  GPIO_ODR_ODR13;
    	}else{
    		GPIOC->ODR &= ~GPIO_ODR_ODR13;
    	}
    	mouse_buttons = left_button | (right_button << 1);

    	hid_config.set_mousex(x_value/4);
    	hid_config.set_mousey(y_value/4);
    	hid_config.set_buttons(mouse_buttons);
   	}
}

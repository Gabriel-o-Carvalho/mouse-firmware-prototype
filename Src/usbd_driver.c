#include "usbd_driver.h"
#include "usb_standards.h"

static uint8_t address_flag = 0;
static uint16_t device_addr = 0;

static void set_stat_tx(uint8_t endpoint_number, uint8_t desired_stat){
	uint16_t *endpoint_reg = ((uint16_t*) USB)+(endpoint_number*2);
	uint16_t write_mask = USB_EPREG_MASK & (*endpoint_reg);
	uint16_t toggle_mask = ((*endpoint_reg) ^ (desired_stat << USB_EP0R_STAT_TX_Pos)) & USB_EP0R_STAT_TX;
	(*endpoint_reg) = write_mask | toggle_mask;
}

static void set_stat_rx(uint8_t endpoint_number, uint8_t desired_stat){
	uint16_t *endpoint_reg = ((uint16_t*) USB)+(endpoint_number*2);
	uint16_t write_mask = USB_EPREG_MASK & (*endpoint_reg);
	uint16_t toggle_mask = ((*endpoint_reg) ^ (desired_stat << USB_EP0R_STAT_RX_Pos)) & USB_EP0R_STAT_RX;
	(*endpoint_reg) = write_mask | toggle_mask;
}

static void set_dtog_tx(uint8_t desired_dtog){
	uint16_t write_mask = USB_EPREG_MASK & USB->EP0R;
	uint16_t toggle_mask = (USB->EP0R ^ (desired_dtog << USB_EP0R_DTOG_TX_Pos)) & USB_EP0R_DTOG_TX;
	USB->EP0R = write_mask | toggle_mask;
}

static void set_dtog_rx(uint8_t desired_dtog){
	uint16_t write_mask = USB_EPREG_MASK & USB->EP0R;
	uint16_t toggle_mask = (USB->EP0R ^ (desired_dtog << USB_EP0R_DTOG_RX_Pos)) & USB_EP0R_DTOG_RX;
	USB->EP0R = write_mask | toggle_mask;
}

static void usbd_driver_initialize(){
	NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);

	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USBEN);

	SET_BIT(USB->CNTR, USB_CNTR_FRES);
	CLEAR_REG(USB->DADDR);
	CLEAR_REG(USB->ISTR);
	CLEAR_REG(USB->BTABLE);
	SET_BIT(USB->CNTR, USB_CNTR_RESETM);

	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

}

static void config_EP1(){
	USB_BFTABLE->ADDR1_TX = 0x50;
	USB_BFTABLE->COUNT1_TX = 0;

	uint16_t ep1_mask = USB_EPREG_MASK & USB->EP1R;

	ep1_mask |= USB_EP1R_EA & 0x01;
	USB->EP1R = ep1_mask;

	ep1_mask |= USB_EP1R_EP_TYPE_1;
	ep1_mask |= USB_EP1R_EP_TYPE_0;
	USB->EP1R = ep1_mask;
}

static void config_EP0(){
	uint32_t table_byte_size = sizeof(USB_BufferTable_TypeDef);
	uint32_t table_hw_size = table_byte_size/2;

	uint32_t tx_buffer_size = 16; // Bytes no STM

	USB_BFTABLE->ADDR0_TX = table_hw_size;
	USB_BFTABLE->COUNT0_TX = 8;
	USB_BFTABLE->ADDR0_RX = table_hw_size+tx_buffer_size/2;
	USB_BFTABLE->COUNT0_RX = (0b000100 << 10);

	USB->EP0R = 0;

	uint16_t ep0_mask = USB_EPREG_MASK & USB->EP0R;
	ep0_mask &= ~USB_EP0R_EP_TYPE_1;
	ep0_mask |= USB_EP0R_EP_TYPE_0;
	USB->EP0R = ep0_mask;
	set_dtog_tx(1);
	set_dtog_rx(0);

	set_stat_rx(0, 0b11);
	set_stat_tx(0, 0b10);
}

static void usbd_driver_reset_handler(void)
{
    USB->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM | USB_CNTR_ERRM;
    USB->DADDR = 0;
    USB->ISTR = 0 ;
    USB->BTABLE = 0;

    USB->DADDR |= USB_DADDR_EF;
    config_EP0();

    usb_events.on_usb_reset_received();

}

static void read_packet(void *buffer, uint16_t size){
	for(int i = 0; i < (size/2); i++, buffer +=2){
		*((uint16_t *) buffer) = (uint16_t)EPDATA->RX0_Buffer[i];
	}
}

static void write_packet(uint8_t endpoint_number, void *buffer, uint16_t data_size){

	uint16_t *bftable = (uint16_t *) USB_BFTABLE;
	*(bftable+2+endpoint_number*8) = data_size;

	switch(endpoint_number){
		case 0x00:
			for(int i=0 ; i < (data_size/2); i++, buffer +=2){
				EPDATA->TX0_Buffer[i] = (uint32_t) *((uint16_t *) buffer);
			}
			break;

		case 0x01:
			for(int i=0 ; i < (data_size/2); i++, buffer +=2){
				EPDATA->TX1_Buffer[i] = (uint32_t) *((uint16_t *) buffer);
			}
			break;
	}

	set_stat_tx(endpoint_number, 0b11);
}

static void set_device_address(uint16_t addr){
	device_addr = addr;
	address_flag = 1;

}

void USB_LP_CAN1_RX0_IRQHandler()
{
    if (USB->ISTR & USB_ISTR_RESET) {       // Reset
        USB->ISTR &= ~USB_ISTR_RESET;
        usbd_driver_reset_handler();

    }else if(READ_BIT(USB->ISTR, USB_ISTR_CTR)){
    	if(!READ_BIT(USB->ISTR, 0xF)){
    		uint16_t ep0_mask;
    		if(USB->EP0R & USB_EP0R_CTR_TX){           // IN Transfer
				ep0_mask = USB->EP0R & USB_EPREG_MASK;
				ep0_mask &= ~USB_EP0R_CTR_TX;
				USB->EP0R = ep0_mask;

				if(address_flag){
					address_flag = 0;
					USB->DADDR |= device_addr;
				}

				set_stat_rx(0,0b11);

				usb_events.on_in_transfer_completed(0);

			}

    		if(READ_BIT(USB->EP0R, USB_EP0R_CTR_RX)){     // OUT/SETUP Transfer
    			ep0_mask = USB->EP0R & USB_EPREG_MASK;
    			ep0_mask &= ~USB_EP0R_CTR_RX;
    			USB->EP0R = ep0_mask;

    			if(READ_BIT(USB->EP0R, USB_EP0R_SETUP)){
    				if(USB->EP0R & USB_EP0R_EP_KIND){
						ep0_mask = USB->EP0R & USB_EPREG_MASK;
						ep0_mask &= ~USB_EP0R_EP_KIND;
						USB->EP0R = ep0_mask;

					}
    				usb_events.on_setup_data_received(0, USB_BFTABLE->COUNT0_RX & 0x3FF);
    			}else {
					usb_events.on_out_transfer_completed(0);
				}
    		}


    	}else if((USB->ISTR & 0xF) == 0x01){
    		uint16_t ep1_mask;

    		if(USB->EP1R & USB_EP1R_CTR_TX){           // IN Transfer
				ep1_mask = USB->EP1R & USB_EPREG_MASK;
				ep1_mask &= ~USB_EP1R_CTR_TX;
				USB->EP1R = ep1_mask;

				usb_events.on_in_transfer_completed(0x01);
			}
    	}

    }else if(READ_BIT(USB->ISTR, USB_ISTR_ERR)){
    	USB->ISTR &= ~USB_ISTR_ERR;
    	usb_events.on_err_transfer_received();
    }

    usb_events.on_usb_polled();
}

UsbDriver usb_driver = {
		.initialize_usb = &usbd_driver_initialize,
		.read_packet = &read_packet,
		.write_packet = &write_packet,
		.set_device_address = &set_device_address,
		.config_endpoint1 = &config_EP1,
};

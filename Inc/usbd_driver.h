#ifndef USBD_DRIVER_H_
#define USBD_DRIVER_H_

#include "stm32f1xx.h"
#include "usb_standards.h"

typedef struct {
	// Buffer 0
	__IO uint16_t ADDR0_TX;
	__IO uint16_t Reserved0;
	__IO uint16_t COUNT0_TX;
	__IO uint16_t Reserved1;
	__IO uint16_t ADDR0_RX;
	__IO uint16_t Reserved2;
	__IO uint16_t COUNT0_RX;
	__IO uint16_t Reserved3;

	// Buffer 1
	__IO uint16_t ADDR1_TX;
	__IO uint16_t Reserved4;
	__IO uint16_t COUNT1_TX;
	__IO uint16_t Reserved5;
	__IO uint16_t ADDR1_RX;
	__IO uint16_t Reserved6;
	__IO uint16_t COUNT1_RX;
	__IO uint16_t Reserved7;

	// Buffer 2
	__IO uint16_t ADDR2_TX;
	__IO uint16_t Reserved8;
	__IO uint16_t COUNT2_TX;
	__IO uint16_t Reserved9;
	__IO uint16_t ADDR2_RX;
	__IO uint16_t Reserved10;
	__IO uint16_t COUNT2_RX;
	__IO uint16_t Reserved11;

	// Buffer 3
	__IO uint16_t ADDR3_TX;
	__IO uint16_t Reserved12;
	__IO uint16_t COUNT3_TX;
	__IO uint16_t Reserved13;
	__IO uint16_t ADDR3_RX;
	__IO uint16_t Reserved14;
	__IO uint16_t COUNT3_RX;
	__IO uint16_t Reserved15;

	// Buffer 4
	__IO uint16_t ADDR4_TX;
	__IO uint16_t Reserved16;
	__IO uint16_t COUNT4_TX;
	__IO uint16_t Reserved17;
	__IO uint16_t ADDR4_RX;
	__IO uint16_t Reserved18;
	__IO uint16_t COUNT4_RX;
	__IO uint16_t Reserved19;

	// Buffer 5
	__IO uint16_t ADDR5_TX;
	__IO uint16_t Reserved20;
	__IO uint16_t COUNT5_TX;
	__IO uint16_t Reserved21;
	__IO uint16_t ADDR5_RX;
	__IO uint16_t Reserved22;
	__IO uint16_t COUNT5_RX;
	__IO uint16_t Reserved23;

	// Buffer 6
	__IO uint16_t ADDR6_TX;
	__IO uint16_t Reserved24;
	__IO uint16_t COUNT6_TX;
	__IO uint16_t Reserved25;
	__IO uint16_t ADDR6_RX;
	__IO uint16_t Reserved26;
	__IO uint16_t COUNT6_RX;
	__IO uint16_t Reserved27;

	// Buffer 7
	__IO uint16_t ADDR7_TX;
	__IO uint16_t Reserved28;
	__IO uint16_t COUNT7_TX;
	__IO uint16_t Reserved29;
	__IO uint16_t ADDR7_RX;
	__IO uint16_t Reserved30;
	__IO uint16_t COUNT7_RX;
	__IO uint16_t Reserved31;

} USB_BufferTable_TypeDef;

typedef struct {
	uint32_t TX0_Buffer[4];
	uint32_t RX0_Buffer[4];
	uint32_t TX1_Buffer[32];

} EPData_TypeDef;

#define BTABLE_OFFSET  0x00000000UL
#define USB_BFTABLE ((USB_BufferTable_TypeDef *)(USB_PMAADDR + BTABLE_OFFSET + BTABLE_OFFSET))

typedef struct {
	void (*initialize_usb)();
	void (*set_address)(uint8_t addr);
	void (*read_packet)(void *buffer, uint16_t size);
	void (*write_packet)(uint8_t endpoint_number, void* buffer, uint16_t data_size);
	void (*set_device_address)(uint16_t addr);
	void (*config_endpoint1)();

} UsbDriver;



#define EPDATA ((EPData_TypeDef*) (USB_PMAADDR + BTABLE_OFFSET + BTABLE_OFFSET + 0x80))

void usbd_initialize();

extern UsbDriver usb_driver;
extern UsbEvents usb_events;

#endif /* USBD_DRIVER_H_ */

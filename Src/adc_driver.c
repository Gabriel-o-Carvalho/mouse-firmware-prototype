#include <stdint.h>
#include <stm32f1xx.h>

#include "adc_driver.h"

volatile uint8_t channel = 0;
volatile uint16_t channels[2] = {0,0};

void adc_init(){
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 | GPIO_CRL_MODE1 | GPIO_CRL_CNF1);

	RCC->CFGR |= RCC_CFGR_ADCPRE;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	ADC1->SQR3 &= ~ADC_SQR3_SQ1;		// Channel 0
	ADC1->SMPR2 |= ADC_SMPR2_SMP0;  // 9.33us sample time

	//ADC1->SQR3 |= ADC_SQR3_SQ2_0;   // Channel 1
	//ADC1->SMPR2 |= ADC_SMPR2_SMP1;  // 9.33us sample time

	ADC1->SQR1 &= ~ADC_SQR1_L;      // 2 Conversions
	//ADC1->SQR1 |=  ADC_SQR1_L_0;

	//ADC1->CR1 |= ADC_CR1_SCAN;

	ADC1->CR2 &= ~ADC_CR2_ALIGN;    // Right alignment

	ADC1->CR1 |= ADC_CR1_EOCIE;
	NVIC_EnableIRQ(ADC1_2_IRQn);

	ADC1->CR2 |= ADC_CR2_CONT;		// Continuous mode
	ADC1->CR2 |= ADC_CR2_ADON;
	for(uint32_t i = 0; i < 10000; i++);

	ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t get_ch_value(uint8_t ch_number){
	return channels[ch_number];
}

void ADC1_2_IRQHandler(){
	channels[channel] = 0x00000FFF & ADC1->DR;

	if(channel == 0){
		channel = 1;
		ADC1->SQR3 &= ~ADC_SQR3_SQ1;
		ADC1->SQR3 |= ADC_SQR3_SQ1_0;   // Channel 1
		ADC1->SMPR2 |= ADC_SMPR2_SMP1;  // 9.33us sample time

	}else{
		channel = 0;
		ADC1->SQR3 &= ~ADC_SQR3_SQ1;
		ADC1->SMPR2 |= ADC_SMPR2_SMP0;  // 9.33us sample time
	}
}

/*
 * pruebaSPI.c
 *   Bme280Spi Main.c
 *  Created on: 12/11/2023
 *      Author: Cristhian Oviedo
 *      brief : Ejemplo de uso del SPI - Pressure sensor MPE280
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "usart_driver_hal.h"
#include "systick_driver_hal.h"

/* Creación de los objetos que controlan algún elemento del proyecto */

GPIO_Handler_t handlerLedState = { 0 };
GPIO_Handler_t handlerUsartRx = { 0 };
GPIO_Handler_t handlerUsartTx = { 0 };

Timer_Handler_t handlerTimer0kState = { 0 };

USART_Handler_t commTerm = { 0 };
uint8_t rxData = '\0';
char printMsg[64] = { 0 };

uint8_t periodicMsg = 0;

/* Para el SPI */
uint8_t spiRxBuffer[6] = { 0 };

/* Prototipos de las funciones */
void initSystems(void);

/* Función principal */
int main(void) {

	/* Configuramos todos los módulos del sistema */

	initSystems();
	mpe280_init();

	/* Imprimimos mensaje de bienvenida */
	usart_writeMsg(&commTerm, "--- Press MPE280 - V0.01 ---\n");

	/* Main loop */
	while(1){
	if(periodicMsg == 4){
	usart_writeMsg(&commTerm, "SPI-Testing \n");

	/* Chip ID*/

	mpe280_readData(MPE280_CHIP_ID, spiRxBuffer, 1);
	sprintf(printMsg, "MPE280_CHIP_ID = 0x%X\n", spiRxBuffer[0]);
	usart_writeMsg(&commTerm, printMsg);

	/* Leyendo un valor xxx*/
	mpe280_readData(MPE280_PRESS_MSB, spiRxBuffer, 3);

	periodicMsg = 0;

	}

	return 0;

	}
	/* Inicialización de todos los periféricos que componen el proyecto */

void initSystems (void){

	/* Inicialización */
	config_SysTick_ms(HSI_CLOCK);

	/*Pin LED OK */

	handlerLedState.pGPIOx = GPIOH;
	handlerLedState.pinConfig.GPIO_PinNumber = PIN_0;
	handlerLedState.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLedState.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerLedState.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	handlerLedState.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerLedState.pinConfig.GPIO_PinAltFunMode = AF14;
	gpio_Config(&handlerLedState);

// Configuramos el Timer0K, para que genere una interrupción periódica.

	 handlerTimer0kState.pTIMx = TIM2;
	 handlerTimer0kState.TIMx_Config.TIMx_Prescaler = 16000;
	 handlerTimer0kState.TIMx_Config.TIMx_Period = 2500;
	 handlerTimer0kState.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	 handlerTimer0kState.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	timer_Config(& handlerTimer0kState);


// Configuramos los pines relacionados al puerto serial
	/* USART Rx */

	handlerUsartRx.pGPIOx = GPIOA;
	handlerUsartRx.pinConfig.GPIO_PinNumber = PIN_3;
	handlerUsartRx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerUsartRx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerUsartRx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	handlerUsartRx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerUsartRx.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&handlerUsartRx);

	/* USART Tx */

	handlerUsartTx.pGPIOx = GPIOA;
	handlerUsartTx.pinConfig.GPIO_PinNumber = PIN_2;
	handlerUsartTx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerUsartTx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerUsartTx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	handlerUsartTx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerUsartTx.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&handlerUsartTx);

	/* Ahora configuramos el periférico */
	commTerm.ptrUSARTx = USART2;
	commTerm.USART_Config.baudrate = USART_BAUDRATE_115200;
	commTerm.USART_Config.datasize = USART_DATASIZE_8BIT;
	commTerm.USART_Config.parity = USART_PARITY_NONE;
	commTerm.USART_Config.stopbits = USART_STOPBIT_1;
	commTerm.USART_Config.mode = USART_MODE_RXTX;
	commTerm.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;

	usart_Config(&commTerm);

	/* pequeña espera para que el USART se configure correctamente. */
	systick_Delay_ms(5);

}

	/* Función Callback ejecutada por la interrupción del TIM2*/
void Timer2_Callback(void){
	gpio_TooglePin(&handlerLedState);
	periodicMsg++;
}

	/* Función Callback que se ejecuta cada vez que se recibe un carácter */
void usart2_RxCallback(void) {
	rxData = usart_getRxData();
}


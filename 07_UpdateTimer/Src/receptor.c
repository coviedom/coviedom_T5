/*
 *  Main de usart
 *
 *   Created on: Oct 31, 2023
 *   	 Author:Cristhian Oviedo
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32_assert.h>
#include <gpio_driver_hal.h>
#include "timer_driver_hal.h"
#include "usart_driver_hal.h"

// Definimos un pin de prueba
GPIO_Handler_t userLed = { 0 };   //PIN A5

//blinky timer
Timer_Handler_t blinkTimer = { 0 };
//comunicacion RS-232 con el PC, ya habilitada en la board Nucleo
//Utiliza la conexion USB
USART_Handler_t receptor = { 0 };
GPIO_Handler_t pinTx = { 0 };
GPIO_Handler_t pinRx = { 0 };
uint8_t sendMsg = 0;
uint8_t RxData = 0;

USART_Handler_t coolterm = { 0 };
GPIO_Handler_t transmcool = { 0 };


char bufferData[64] = { 0 };

//Definicion de las caberas de las funciones del main
void initSystem(void);

/*
 * The main function, where everything happens.
 */
int main(void) {
	//iniciamos los elementos del sistema
	initSystem();

	//Ciclo principal
	while (1) {
//		if (sendMsg) {
//			sendMsg = 0;
//			sprintf(bufferData, "El Usart Funciona Bien!! %d \n\r", 700);
//			usart_writeMsg(&receptor, bufferData);
//		}
		if(RxData != '\0') {
			if (RxData == 'd' ) {
//				sprintf(bufferData, "El Usart Funciona Bien!! %d \n\r",700);
//				usart_writeMsg(&coolterm, bufferData);
				gpio_TooglePin(&userLed);

//				RxData = 0;
//				sendMsg = 1;
			}
			RxData = 0;

		}

	}

}

void initSystem(void) {

	/* Configuramos el pin*/
	userLed.pGPIOx = GPIOA;
	userLed.pinConfig.GPIO_PinNumber = PIN_5;
	userLed.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	//Cargamos la configuracion en los registros que gobiernan el puerto
	gpio_Config(&userLed);
	gpio_WritePin(&userLed, SET);

	//Configuracion del TIM2
	blinkTimer.pTIMx = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler = 16000; // Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period = 1000; // De la mano con el prescaler
	blinkTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	// configuramos el timer
	timer_Config(&blinkTimer);
	//encendemos el timer
	timer_SetState(&blinkTimer, TIMER_ON);

	//configuramos los pines del puerto serial
//	//Pin sobre los que funciona el USART2 (TX)
	pinTx.pGPIOx = GPIOA;
	pinTx.pinConfig.GPIO_PinNumber = PIN_11;
	pinTx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinAltFunMode = AF8;
	pinTx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinTx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	gpio_Config(&pinTx);
	//Pin sobre los que funciona el USART2 (RX)
	pinRx.pGPIOx = GPIOA;
	pinRx.pinConfig.GPIO_PinNumber = PIN_12;
	pinRx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinAltFunMode = AF8;
	pinRx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinRx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	gpio_Config(&pinRx);
	//Configuramos el puerto serial (USART2)
	receptor.ptrUSARTx = USART6;
	receptor.USART_Config.baudrate = USART_BAUDRATE_9600;
	receptor.USART_Config.datasize = USART_DATASIZE_8BIT;
	receptor.USART_Config.parity = USART_PARITY_NONE;
	receptor.USART_Config.stopbits = USART_STOPBIT_1;
	receptor.USART_Config.mode = USART_MODE_RXTX;
	receptor.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart_Config(&receptor);

	//configuramos los pines del puerto serial
	//Pin sobre los que funciona el USART2 (TX)
//	transmcool.pGPIOx = GPIOA;
//	transmcool.pinConfig.GPIO_PinNumber = PIN_2;
//	transmcool.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//	transmcool.pinConfig.GPIO_PinAltFunMode = AF7;
//	transmcool.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	transmcool.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
//	gpio_Config(&transmcool);
//	//Pin sobre los que funciona el USART2 (RX)
//
//	//Configuramos el puerto serial (USART2)
//	coolterm.ptrUSARTx = USART2;
//	coolterm.USART_Config.baudrate = USART_BAUDRATE_9600;
//	coolterm.USART_Config.datasize = USART_DATASIZE_8BIT;
//	coolterm.USART_Config.parity = USART_PARITY_NONE;
//	coolterm.USART_Config.stopbits = USART_STOPBIT_1;
//	coolterm.USART_Config.mode = USART_MODE_RXTX;
//	coolterm.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
//	usart_Config(&coolterm);

}

//Overwrite function

void Timer2_Callback(void) {
//	gpio_TooglePin(&userLed);


}

void usart6_RxCallback(void){
	//leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto además debe bajar la bandera de la interrupción.
	RxData = usart_getRxData();

}
//funcion para detectar problemas de parametros incorrectos
//al momento de ejecutar un programa

void assert_failed(uint8_t *file, uint32_t line) {
	while (1) {
		// problemas...
	}
}
/*
 * receptor.c
 *
 *  Created on: 1/12/2023
 *      Author: ingfisica
 */



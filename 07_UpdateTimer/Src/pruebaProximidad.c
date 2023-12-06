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

GPIO_Handler_t infrarojo_delantero = {0};


void initSystem(void);

/*
 * The main function, where everything happens.
 */
int main(void) {
	//iniciamos los elementos del sistema
	initSystem();

	//Ciclo principal
	while (1) {

		uint32_t read1 = gpio_ReadPin(&infrarojo_delantero);
		/*Entonces si está en 1 es por que esta encendido el de las resoluciones ya que es de catodo comun*/
		if (read1 == RESET) {
			gpio_WritePin(&userLed, SET);
		}
		else {
			gpio_WritePin(&userLed, RESET);
		}

	}

}

void initSystem(void) {

	/* Configuramos el pin*/
	userLed.pGPIOx = GPIOC;
	userLed.pinConfig.GPIO_PinNumber = PIN_1;
	userLed.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	//Cargamos la configuracion en los registros que gobiernan el puerto
	gpio_Config(&userLed);


	//Configuracion del TIM2
	blinkTimer.pTIMx = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler = 16000; // Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period = 250; // De la mano con el prescaler
	blinkTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	// configuramos el timer
	timer_Config(&blinkTimer);
	//encendemos el timer
	timer_SetState(&blinkTimer, TIMER_ON);

	infrarojo_delantero.pGPIOx = GPIOB;
	infrarojo_delantero.pinConfig.GPIO_PinNumber = PIN_5;
	infrarojo_delantero.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	infrarojo_delantero.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	infrarojo_delantero.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	infrarojo_delantero.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&infrarojo_delantero);

}


//void Timer2_Callback(void) {
//	gpio_TooglePin(&userLed);
//
//
//}





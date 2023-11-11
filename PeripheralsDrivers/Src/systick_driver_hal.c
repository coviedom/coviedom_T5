/*
 * systick_driver_hal.c
 *
 *  Created on: Nov 7, 2023
 *      Author: Cristhian Oviedo
 *
 *  Este driver controla el timer por defecto todo procesador ARM Cortex Mx el cual
 *  hace parte del sistema independiente de la empresa fabricaten del MCU.
 *
 *  para encontrar cual  es el registro de configuracion debemos utilizar el manual generico de
 *  prosesador Cortex - M4, ya que es alli donde se encuentra ya que alli donde se encuentra la
 *  dumentacion para este periferico
 *
 *  en el archivo core_cam4.h, la estructura que define el periferico se llama Systick_Type

 */

#include <stm32f4xx.h>
#include "systick_driver_hal.h"

uint64_t ticks = 0;
uint64_t ticks_start = 0;
uint64_t ticks_counting = 0;

void config_SysTick_ms(uint8_t systemClock){

	ticks = 0;
	//cargando el valor del limite de incremento que represente 1ms
	switch(systemClock){

	//Caso para el reloj HSI -> 16MHz
	case 0:
		SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
		break;
	//Caso para el reloj HSE
	case 1:
	    SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
		break;
	//caso para el reloj PLL a 100MHz
	case 2:
	    SysTick->LOAD = SYSTICK_LOAD_VALUE_100MHz_1ms;
		break;
	default:
	    SysTick->LOAD = SYSTICK_LOAD_VALUE_100MHz_1ms;
		break;

	}


     //limpiamos el valor actual del SysTick
     SysTick->VAL = 0;

     //configuramos el reloj interno como el reloj para el timer
     SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

     //desactivamos las interrupciones globales
     __disable_irq();

     //Matriculamos la interrupciones en el NVIC
      NVIC_EnableIRQ(SysTick_IRQn);

      // Activamos la interrupcion debida al conteo a cero del SysTick
      SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

      // activamos el timer
      SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

      // activamos de nuevo las interrupciones globales
      __enable_irq();

 }

 uint64_t obtener_ticks_ms(void){
 	return ticks;
 }

 void systick_Delay_ms(uint32_t wait_time_ms){
 	/*se toma primer valor de tiempo para comparar*/
 	ticks_start = obtener_ticks_ms();

 	/*se tiene el segundo valor del tiempo para comparar*/
 	ticks_counting = obtener_ticks_ms();
 	//compara: si el valor "caunting" es menor que el "start + wait"
 	//actualiza el valor "counting"
 	//repite esta operacion hasta que counting sea mayor (se cumple el tiempo de espera)
 	while(ticks_counting < (ticks_start + (uint64_t)wait_time_ms)){
 		//actualizar el valor
 		ticks_counting = obtener_ticks_ms();
 	}

 }

 void SysTick_Handler(void){
 	// verificamos que la interrupcion se lanzo
 	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk ){

 		//limpiamos la bandera
 		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;

 		// incrementamos en 1 el contador
 		ticks++;
 	}

 }


/*
 * systick_driver_hal.h
 *
 *  Created on: Nov 7, 2023
 *      Author: Cristhian Oviedo
 */

#ifndef SYSTICK_DRIVER_HAL_H_
#define SYSTICK_DRIVER_HAL_H_

#include <stm32f4xx.h>
#include <stdio.h>

#define SYSTICK_LOAD_VALUE_16MHz_1ms 16000 //numero de ciclos en 1ms
#define SYSTICK_LOAD_VALUE_100MHz_1ms 100000 //numero de ciclos en 1ms

void config_SysTick_ms(uint8_t systemClock);
void systick_Delay_ms(uint32_t wait_time_ms);
uint64_t obtener_ticks_ms(void);

#endif /* SYSTICK_DRIVER_HAL_H_ */

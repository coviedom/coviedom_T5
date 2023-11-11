/*
 *  Main de SYSTICK
 *
 *   Created on: Nov 7, 2023
 *   	 Author:Cristhian Oviedo
 */
#include <stdint.h>
#include <stm32f4xx.h>
#include <stdbool.h>
#include "timer_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "stm32_assert.h"
#include "exti_driver_hal.h"
#include "systick_driver_hal.h"
#include "gpio_driver_hal.h"


void start(void);

#define  HSI_CLOCK_CONFIGURED  0
#define  HSE_CLOCK_CONFIGURED 1
#define  PLL_CLOCK_CONFIGURED 2

GPIO_Handler_t _LedBlinky = { 0 };  //pb10

Timer_Handler_t _BlinkyTimer3 = { 0 };

uint8_t bandera_timer = 0;

int main(void) {
	start();
	config_SysTick_ms(HSI_CLOCK_CONFIGURED);

	while (1) {
		if (bandera_timer > 1){
			//pruebas al systick
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(700);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(700);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(700);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(700);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(700);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(700);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(700);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(700);

			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(10000);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(10000);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(10000);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(10000);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(10000);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(10000);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(10000);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(10000);

			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(180);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(180);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(180);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(180);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(180);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(180);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(180);
			gpio_TooglePin(&_LedBlinky);
			systick_Delay_ms(180);

			bandera_timer=0;

		}
	}
}
void start(void){
	_LedBlinky.pGPIOx = GPIOB;
	_LedBlinky.pinConfig.GPIO_PinNumber = PIN_10;
	_LedBlinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_LedBlinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_LedBlinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_LedBlinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_LedBlinky);

	_BlinkyTimer3.pTIMx = TIM3;
	_BlinkyTimer3.TIMx_Config.TIMx_Prescaler = 16000;
	_BlinkyTimer3.TIMx_Config.TIMx_Period = 250;
	_BlinkyTimer3.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	_BlinkyTimer3.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	timer_Config(&_BlinkyTimer3);
	timer_SetState(&_BlinkyTimer3, TIMER_ON);
}

void Timer3_Callback(void){
	bandera_timer ++;
}


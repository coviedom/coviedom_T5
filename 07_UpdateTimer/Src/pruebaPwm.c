/*
 *  Main de pwm
 *
 *   Created on: nov 7, 2023
 *   	 Author:Cristhian Oviedo
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include <stdbool.h>
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "systick_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "stm32_assert.h"
#include "pwm_driver_hal.h"
#include "exti_driver_hal.h"

void start(void);

GPIO_Handler_t _LedBlinky = { 0 };  //pb10
Timer_Handler_t _BlinkyTimer3 = { 0 };

GPIO_Handler_t _Tx2 = { 0 };
GPIO_Handler_t _Rx2 = { 0 };
USART_Handler_t Usart2 = { 0 };
uint8_t teclado = 0;

/*Definimos lo relacionado al PWM*/
GPIO_Handler_t _Pwm_pin = { 0 };
PWM_Handler_t _PWM_Muestreo = {0};

/*Definimos el valor del dutty*/
uint16_t _Duty = 500;
/*Definimos el arreglo en el que se guardará el mensaje*/
char buffer_info[128] = {0};



int main(void) {	start();
	config_SysTick_ms(0);

	while (1) {
		if (teclado != '\0'){
			if (teclado == 'd'){
				_Duty -= 50;
				if (_Duty < 50){
					_Duty = 50;
				}
				sprintf(buffer_info, "Dutty =%u \n",(unsigned int)_Duty);
				usart_writeMsg(&Usart2, buffer_info);

			}
			else if (teclado == 'a'){
				_Duty += 50;
				if (_Duty > 1000){
					_Duty = 1000;
				}
				sprintf(buffer_info, "Dutty =%u \n",(unsigned int)_Duty);
				usart_writeMsg(&Usart2, buffer_info);

			}
			teclado = 0;
			actualiza_Ciclo_Duty(&_PWM_Muestreo, _Duty);


		}
	}
}

void start(void){
	_LedBlinky.pGPIOx = GPIOA;
	_LedBlinky.pinConfig.GPIO_PinNumber = PIN_10;
	_LedBlinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_LedBlinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_LedBlinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_LedBlinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_LedBlinky);

	_BlinkyTimer3.pTIMx = TIM9;
	_BlinkyTimer3.TIMx_Config.TIMx_Prescaler = 16000;
	_BlinkyTimer3.TIMx_Config.TIMx_Period = 250;
	_BlinkyTimer3.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	_BlinkyTimer3.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	timer_Config(&_BlinkyTimer3);
	timer_SetState(&_BlinkyTimer3, TIMER_ON);

	Usart2.ptrUSARTx = USART2;
	Usart2.USART_Config.baudrate = USART_BAUDRATE_115200;
	Usart2.USART_Config.datasize = USART_DATASIZE_8BIT;
	Usart2.USART_Config.parity = USART_PARITY_NONE;
	Usart2.USART_Config.stopbits = USART_STOPBIT_1;
	Usart2.USART_Config.mode = USART_MODE_RXTX;
	Usart2.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;

	usart_Config(&Usart2);

	_Tx2.pGPIOx = GPIOA;
	_Tx2.pinConfig.GPIO_PinNumber = PIN_2;
	_Tx2.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_Tx2.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Tx2.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	_Tx2.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_Tx2.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&_Tx2);

	_Rx2.pGPIOx = GPIOA;
	_Rx2.pinConfig.GPIO_PinNumber = PIN_3;
	_Rx2.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_Rx2.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Rx2.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	_Rx2.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_Rx2.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&_Rx2);

	_Pwm_pin.pGPIOx = GPIOC;
	_Pwm_pin.pinConfig.GPIO_PinNumber = PIN_7;
    _Pwm_pin.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_Pwm_pin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Pwm_pin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_Pwm_pin.pinConfig.GPIO_PinAltFunMode = AF2;

	gpio_Config(&_Pwm_pin);


	_PWM_Muestreo.pTIMx = TIM3;
	_PWM_Muestreo.config.Canal = PWM_CHANNEL_2;
	_PWM_Muestreo.config.prescaler = 16;
	_PWM_Muestreo.config.periodo = 25;
	_PWM_Muestreo.config.CicloDuty = 10;
	configuracion_del_pwm(&_PWM_Muestreo);

	activar_salida(&_PWM_Muestreo);
	inicio_de_señal_pwm(&_PWM_Muestreo);
}

/*Con esta funcion de interrupcion se cambia el estado del led dependiendo del ARR*/
void Timer9_Callback(void){
	gpio_TooglePin(&_LedBlinky);
}
/*Funcion que si se recibe algo por comunicacion serial almacena la informacion en la variable teclado*/
void usart2_RxCallback(void) {
	teclado = usart_getRxData();
}

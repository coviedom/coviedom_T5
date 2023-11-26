/**
 ******************************************************************************
 * @file           : Main del Maestro.c
 * @author         : Cristhian Andrés Oviedo Montalvo
 * @brief          : Proyecto WALL-E
 * Fecha           : 25 de Noviembre 2023
 ******************************************************************************
*/
#include <stdint.h>
#include <stdio.h>
#include <stm32f4xx.h>
#include <math.h>
#include <stdbool.h>
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "exti_driver_hal.h"
#include "pwm_driver_hal.h"
#include "timer_driver_hal.h"
#include "arm_math.h"
/*******************************************************************************************************************************************/

/*Se encabeza la funcion a utilizar en el programa*/
void start(void);
/*Se define el Led rojo del blinky*/
GPIO_Handler_t led_Blinky = {0};         //PB10
/*Se define el timer a utilizar*/
Timer_Handler_t timer_del_Blinky = {0};
/*Se define el Usart y sus respectivos pines*/
GPIO_Handler_t Transmisor = {0};   //PA2
GPIO_Handler_t Receptor = {0};     //PA3
USART_Handler_t usart = {0};
uint8_t teclado = 0; /*variable que guarda el caracter recibido tras presionar una tecla*/
/*Definimos el arreglo en el que se guardará el mensaje para cool Term*/
char buffer_info[128] = {0};

uint16_t _Duty = 500;
PWM_Handler_t _PWM_ENA = {0};
GPIO_Handler_t _ENA = {0};
GPIO_Handler_t _IN1 = {0};
GPIO_Handler_t _IN2 = {0};

uint16_t _Duty2 = 500;
PWM_Handler_t _PWM_ENB = {0};
GPIO_Handler_t _ENB = {0};
GPIO_Handler_t _IN3 = {0};
GPIO_Handler_t _IN4 = {0};

int main(void) {
	/*Se activa el co-procesador FPU*/
	SCB->CPACR |= (0xF << 20);
	/*funcion que se encarga de configurar las definiciones antes mencionadas*/
	start();
	/*Lo que realiza el codigo ciclicamente*/
	while (1) {
/*******************************************************************************************************************************************/

		/*Lo siguiente ocurre si presiona una letra del teclado*/
		if (teclado != '\0') {
			if (teclado == 'x'){
				_Duty -= 50;
				_Duty2 -= 50;
				if (_Duty < 50 || _Duty2 < 50){
					_Duty = 50;
					_Duty2 = 50;

				}
				sprintf(buffer_info, "Dutty =%u \n",(unsigned int)_Duty);
				usart_writeMsg(&usart, buffer_info);
				teclado = 0;

			}
			if (teclado == 'z'){
				_Duty += 50;
				_Duty2 += 50;
				if (_Duty > 1000 || _Duty2 > 1000){
					_Duty = 1000;
					_Duty2 = 1000;
				}
				sprintf(buffer_info, "Dutty =%u \n",(unsigned int)_Duty);
				usart_writeMsg(&usart, buffer_info);
				teclado = 0;

			}
			actualiza_Ciclo_Duty(&_PWM_ENB, _Duty2);
			actualiza_Ciclo_Duty(&_PWM_ENA, _Duty);


			/*Si se presiona la letra "a"*/
			if (teclado == 'w') {
				usart_writeMsg(&usart,"Adelante\n\r");
				gpio_WritePin(&_IN1,RESET);
				gpio_WritePin(&_IN2,SET);
				gpio_WritePin(&_IN3,RESET);
				gpio_WritePin(&_IN4,SET);
				teclado = 0;
			}
			/*Si se presiona la letra "b"*/
			else if (teclado == 's') {
				usart_writeMsg(&usart,"Atrás\n\r");
				gpio_WritePin(&_IN1,SET);
				gpio_WritePin(&_IN2,RESET);
				gpio_WritePin(&_IN3,SET);
				gpio_WritePin(&_IN4,RESET);
				teclado = 0;
			}

			else if (teclado == 'a') {
				usart_writeMsg(&usart,"Izquierda\n\r");
				gpio_WritePin(&_IN1,RESET);
				gpio_WritePin(&_IN2,RESET);
				gpio_WritePin(&_IN3,RESET);
				gpio_WritePin(&_IN4,SET);
				teclado = 0;
			}
			/*Si se presiona la letra "b"*/
			else if (teclado == 'd') {
				usart_writeMsg(&usart,"Derecha\n\r");
				gpio_WritePin(&_IN1,RESET);
				gpio_WritePin(&_IN2,SET);
				gpio_WritePin(&_IN3,RESET);
				gpio_WritePin(&_IN4,RESET);
				teclado = 0;
			}
			else if (teclado == 'g') {
				usart_writeMsg(&usart,"360 a la Izquierda\n\r");
				gpio_WritePin(&_IN1,SET);
				gpio_WritePin(&_IN2,RESET);
				gpio_WritePin(&_IN3,RESET);
				gpio_WritePin(&_IN4,SET);
				teclado = 0;
			}
			else if (teclado == 'h') {
				usart_writeMsg(&usart,"360 a la derecha\n\r");
				gpio_WritePin(&_IN1,RESET);
				gpio_WritePin(&_IN2,SET);
				gpio_WritePin(&_IN3,SET);
				gpio_WritePin(&_IN4,RESET);
				teclado = 0;
			}
			else if (teclado == 'o') {
				usart_writeMsg(&usart,"APAGA\n\r");
				gpio_WritePin(&_IN1,RESET);
				gpio_WritePin(&_IN2,RESET);
				gpio_WritePin(&_IN3,RESET);
				gpio_WritePin(&_IN4,RESET);
				teclado = 0;
			}
			else if (teclado == 'r') {
				usart_writeMsg(&usart,"Funciona\n\r");
				teclado = 0;
			}
			/*Si se presiona la letra "c"**/
		}
	}
}
/*******************************************************************************************************************************************/
void start(void) {
	/*Se configura el led rojo del blinky*/
//	led_Blinky.pGPIOx = GPIOB;
//	led_Blinky.pinConfig.GPIO_PinNumber = PIN_10;
//	led_Blinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
//	led_Blinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
//	led_Blinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
//	led_Blinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	gpio_Config(&led_Blinky);
	/*Se configura el timer para el blinky*/
//	timer_del_Blinky.pTIMx = TIM9;
//	timer_del_Blinky.TIMx_Config.TIMx_Prescaler = 16000;
//	timer_del_Blinky.TIMx_Config.TIMx_Period = 250;
//	timer_del_Blinky.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
//	timer_del_Blinky.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
//	timer_Config(&timer_del_Blinky);
	/*Que el timer esté encendido*/
//	timer_SetState(&timer_del_Blinky, TIMER_ON);
	/*Se configura el usart de comunicacion serial*/
	usart.ptrUSARTx = USART2;
	usart.USART_Config.baudrate = USART_BAUDRATE_115200;
	usart.USART_Config.datasize = USART_DATASIZE_8BIT;
	usart.USART_Config.parity = USART_PARITY_NONE;
	usart.USART_Config.stopbits = USART_STOPBIT_1;
	usart.USART_Config.mode = USART_MODE_RXTX;
	usart.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart_Config(&usart);
	/*Se configura el pin transmisor*/
	Transmisor.pGPIOx = GPIOA;
	Transmisor.pinConfig.GPIO_PinNumber = PIN_2;
	Transmisor.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Transmisor.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Transmisor.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	Transmisor.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Transmisor.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&Transmisor);
	/*Se configura el pin receptor*/
	Receptor.pGPIOx = GPIOA;
	Receptor.pinConfig.GPIO_PinNumber = PIN_3;
	Receptor.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Receptor.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Receptor.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	Receptor.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Receptor.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&Receptor);

	_ENB.pGPIOx = GPIOC;
	_ENB.pinConfig.GPIO_PinNumber = PIN_7;
    _ENB.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_ENB.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_ENB.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_ENB.pinConfig.GPIO_PinAltFunMode = AF2;
	gpio_Config(&_ENB);

	_PWM_ENB.pTIMx = TIM3;
	_PWM_ENB.config.Canal = PWM_CHANNEL_2;
	_PWM_ENB.config.prescaler = 16;
	_PWM_ENB.config.periodo = 1000;
	_PWM_ENB.config.CicloDuty = 500;
	configuracion_del_pwm(&_PWM_ENB);
	activar_salida(&_PWM_ENB);
	inicio_de_señal_pwm(&_PWM_ENB);

	_IN4.pGPIOx = GPIOA;
	_IN4.pinConfig.GPIO_PinNumber = PIN_12;
	_IN4.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_IN4.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_IN4.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_IN4.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_IN4);
	gpio_WritePin(&_IN4,RESET);

	_IN3.pGPIOx = GPIOC;
	_IN3.pinConfig.GPIO_PinNumber = PIN_5;
	_IN3.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_IN3.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_IN3.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_IN3.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_IN3);
	gpio_WritePin(&_IN3,RESET);


	_IN2.pGPIOx = GPIOC;
	_IN2.pinConfig.GPIO_PinNumber = PIN_9;
	_IN2.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_IN2.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_IN2.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_IN2.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_IN2);
	gpio_WritePin(&_IN2,RESET);

	_IN1.pGPIOx = GPIOB;
	_IN1.pinConfig.GPIO_PinNumber = PIN_6;
	_IN1.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_IN1.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_IN1.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_IN1.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_IN1);
	gpio_WritePin(&_IN1,RESET);

	_ENA.pGPIOx = GPIOA;
	_ENA.pinConfig.GPIO_PinNumber = PIN_5;
    _ENA.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_ENA.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_ENA.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_ENA.pinConfig.GPIO_PinAltFunMode = AF1;
	gpio_Config(&_ENA);

	_PWM_ENA.pTIMx = TIM2;
	_PWM_ENA.config.Canal = PWM_CHANNEL_1;
	_PWM_ENA.config.prescaler = 16;
	_PWM_ENA.config.periodo = 1000;
	_PWM_ENA.config.CicloDuty = 500;
	configuracion_del_pwm(&_PWM_ENA);
	activar_salida(&_PWM_ENA);
	inicio_de_señal_pwm(&_PWM_ENA);


}
/*******************************************************************************************************************************************/

/*Con esta funcion de interrupcion se cambia el estado del led dependiendo del ARR*/
void Timer9_Callback(void) {
//	gpio_TooglePin(&led_Blinky);
}
/*Funcion que si se recibe algo por comunicacion serial almacena la informacion en la variable teclado*/
void usart2_RxCallback(void) {
	teclado = usart_getRxData();
}
/*Esta funcion se activa por cada conversion segun la secuencia*/
void adc_CompleteCallback(void) {
	__NOP();
}
/*FINISH*//*********************************************************************************************************************************/

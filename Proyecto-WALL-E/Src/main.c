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

/*Se hace los #define para calcular la frecuencia*/

#define CANTIDAD_DE_SENSORES 2
#define FRECUENCIA_DE_MUESTREO 40000
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
/*Se define los tres sensores tipo ADC_Config_t*/
ADC_Config_t _Sensor1 = {0};  //PB1
ADC_Config_t _Sensor2 = {0};  //PB0

/*Se crea un arreglo que va a contener los sensores*/
ADC_Config_t _Sensores[CANTIDAD_DE_SENSORES]= {0};
/*Se crea el PWM que va ayudar a muestrear las señales*/



//float32_t señal_A[TAMAÑO_DE_DATOS]; /*Arreglo de datos despues de muestrear la señal con el PWM*/
//
//float32_t señal_B[TAMAÑO_DE_DATOS]; /*Arreglo de datos despues de muestrear la señal con el PWM*/


/*******************************************************************************************************************************************/
/*Para la señal "A" Se define lo que nos ayudara a inciar y terminar la recoleccion de los datos*/
uint16_t tecla_A = 0; /*Esta bandera inicia la recoleccion de los datos a la vez que se enciende el PWM*/
uint16_t bandera_final_A = 0; /*Esta bandera indica que se ha termidado de recolectar los datos*/
uint16_t contad_A = 0; /*Este contador es quien ayuda a recoger los datos hasta 512*/
/*******************************************************************************************************************************************/
/*Para la señal "B" Se define lo que nos ayudara a inciar y terminar la recoleccion de los datos*/
uint16_t tecla_B = 0;/*Esta bandera inicia la recoleccion de los datos a la vez que se enciende el PWM*/
uint16_t bandera_final_B = 0; /*Esta bandera indica que se ha termidado de recolectar los datos*/
uint16_t contad_B = 0;  /*Este contador es quien ayuda a recoger los datos hasta 512*/
/*******************************************************************************************************************************************/

/*********************************************************************************/
uint8_t _Contador_Secuencia = 0; /*Contador de la secuencia de conversion*/

uint16_t _Duty = 500;
PWM_Handler_t _PWM_ENA = {0};
GPIO_Handler_t _ENA = {0};
GPIO_Handler_t _IN1 = {0};
GPIO_Handler_t _IN2 = {0};

int main(void) {
	/*Se activa el co-procesador FPU*/
	SCB->CPACR |= (0xF << 20);
	/*funcion que se encarga de configurar las definiciones antes mencionadas*/
	start();
	/*Lo que realiza el codigo ciclicamente*/
	while (1) {
/*******************************************************************************************************************************************/

/*******************************************************************************************************************************************/
        /*Lo siguiente ocurre si presiona una letra del teclado*/
		if (teclado != '\0') {
			if (teclado == 'x'){
				_Duty -= 50;
				if (_Duty < 50){
					_Duty = 50;
				}
				sprintf(buffer_info, "Dutty =%u \n",(unsigned int)_Duty);
				usart_writeMsg(&usart, buffer_info);
				teclado = 0;

			}
			if (teclado == 'a'){
				_Duty += 50;
				if (_Duty > 1000){
					_Duty = 1000;
				}
				sprintf(buffer_info, "Dutty =%u \n",(unsigned int)_Duty);
				usart_writeMsg(&usart, buffer_info);
				teclado = 0;

			}

			actualiza_Ciclo_Duty(&_PWM_ENA, _Duty);

			/*Si se presiona la letra "a"*/
			if (teclado == 'b') {
				usart_writeMsg(&usart,"prende IN1\n\r");

				gpio_WritePin(&_IN1,SET);

				teclado = 0;
			}
			/*Si se presiona la letra "b"*/
			if (teclado == 'c') {
				usart_writeMsg(&usart,"apaga IN1\n\r");
				gpio_WritePin(&_IN1,RESET);

				/*Se enciende el PWM de muestreo*/
//				inicio_de_señal_pwm(&_PWM_Muestreo);
				/*Se sube la bandera que inicia la recoleccion de datos para la señal del sensor 2*/
//				tecla_B = SET;
				/*Se hace cero a teclado para poder volver a ingresar la letra si asi lo quiere*/
				teclado = 0;
			}

			if (teclado == 'd') {
				usart_writeMsg(&usart,"prende IN2\n\r");
				gpio_WritePin(&_IN2,SET);
				teclado = 0;
			}
			/*Si se presiona la letra "b"*/
			if (teclado == 'e') {
				usart_writeMsg(&usart,"apaga IN2\n\r");
				gpio_WritePin(&_IN2,RESET);
				teclado = 0;
			}
			if (teclado == 'r') {
				usart_writeMsg(&usart,"funciona\n\r");
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
	/*Enseguida se van a configurar los 3 canales ADC*/
	/*El sensor 1 siendo el primero de la secuencia*/
//	_Sensor1.channel = CHANNEL_9;
//	_Sensor1.resolution = RESOLUTION_12_BIT;
//	_Sensor1.dataAlignment = ALIGNMENT_RIGHT;
//	_Sensor1.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
//	_Sensor1.interrupState = ADC_INT_ENABLE;
//	/*El sensor 2 siendo el segundo de la secuencia*/
//	_Sensor2.channel = CHANNEL_8;
//	_Sensor2.resolution = RESOLUTION_12_BIT;
//	_Sensor2.dataAlignment = ALIGNMENT_RIGHT;
//	_Sensor2.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
//	_Sensor2.interrupState = ADC_INT_ENABLE;
//	/*El sensor 3 siendo el tercero de la secuencia*/
//
//	/*se asigna la posicion en el arreglo segun la secuencia */
//	_Sensores[0] = _Sensor1;
//	_Sensores[1] = _Sensor2;

	/*Ahora que se cargue la configuracion de todos los sensores*/
//	adc_ConfigMultichannel (_Sensores,CANTIDAD_DE_SENSORES);
	/*Se configura el PWM que va a ayudar a muestrear la señal */



	_ENA.pGPIOx = GPIOC;
	_ENA.pinConfig.GPIO_PinNumber = PIN_7;
    _ENA.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_ENA.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_ENA.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_ENA.pinConfig.GPIO_PinAltFunMode = AF2;
	gpio_Config(&_ENA);

	_PWM_ENA.pTIMx = TIM3;
	_PWM_ENA.config.Canal = PWM_CHANNEL_2;
	_PWM_ENA.config.prescaler = 16;
	_PWM_ENA.config.periodo = 1000;
	_PWM_ENA.config.CicloDuty = 500;
	configuracion_del_pwm(&_PWM_ENA);
	activar_salida(&_PWM_ENA);
	inicio_de_señal_pwm(&_PWM_ENA);

	_IN1.pGPIOx = GPIOA;
	_IN1.pinConfig.GPIO_PinNumber = PIN_12;
	_IN1.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_IN1.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_IN1.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_IN1.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_IN1);
	gpio_WritePin(&_IN1,RESET);

	_IN2.pGPIOx = GPIOC;
	_IN2.pinConfig.GPIO_PinNumber = PIN_5;
	_IN2.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_IN2.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_IN2.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_IN2.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_IN2);
	gpio_WritePin(&_IN2,RESET);


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
//	/*Dependiendo de la secuencia de conversion, es decir del Contador Secuencia, se hace la toma del dato*/
//	_Sensores[_Contador_Secuencia].adcData = adc_GetValue();
//
///*******************************************************************************************************************************************/
//	/*Si se ha presionado la tecla "a" ocurrira lo siguiente */
//	if (tecla_A) {
//		/*Se evalua si se acaba de hacer una sola conversion del sensor 1*/
//		if (_Contador_Secuencia == 0) {
//			/*Se almacena secuencialmente el dato ADC del sensor 1 en el arreglo de 512 datos llamado Señal A*/
//			señal_A[contad_A] = _Sensores[0].adcData;
//			/*Se incrementa este contador con el fin de llenar el arreglo hasta 512*/
//			contad_A++;
//			/*Se evalua si se llega a 512 datos*/
//			if (contad_A == TAMAÑO_DE_DATOS) {
//				/*Se reinicia el contador en caso de que quiera por accion de la tecla "a" volver a recolectar datos*/
//				contad_A = 0;
//				/*Se levanta la bandera para que no se siga recolectando los datos en este arreglo Señal A*/
//				tecla_A = RESET;
//				/*Se levanta la bandera indicando que se terminó la recolección*/
//				bandera_final_A = SET;
//			}
//		}
//	}
///*******************************************************************************************************************************************/
//	/*Si se ha presionado la tecla "b" ocurrira lo siguiente */
//	if (tecla_B) {
//		/*Se evalua si se acaba de hacer una sola conversion del sensor 2*/
//		if (_Contador_Secuencia == 1) {
//			/*Se almacena secuencialmente el dato ADC del sensor 2 en el arreglo de 512 datos llamado Señal B*/
//			señal_B[contad_B] = _Sensores[1].adcData;
//			/*Se incrementa este contador con el fin de llenar el arreglo hasta 512*/
//			contad_B++;
//			/*Se evalua si se llega a 512 datos*/
//			if (contad_B == TAMAÑO_DE_DATOS) {
//				/*Se reinicia el contador en caso de que quiera por accion de la tecla "b" volver a recolectar datos*/
//				contad_B = 0;
//				/*Se levanta la bandera para que no se siga recolectando los datos en este arreglo Señal B*/
//				tecla_B = RESET;
//				/*Se levanta la bandera indicando que se terminó la recolección*/
//				bandera_final_B = SET;
//			}
//		}
//	}
///*******************************************************************************************************************************************/
//	/*Si se ha presionado la tecla "c" ocurrira lo siguiente */
//	if (tecla_C) {
//		/*Se evalua si se acaba de hacer una sola conversion del sensor 3*/
//		if (_Contador_Secuencia == 2) {
//			/*Se almacena secuencialmente el dato ADC del sensor 3 en el arreglo de 512 datos llamado Señal C*/
//			señal_C[contad_C] = _Sensores[2].adcData;
//			/*Se incrementa este contador con el fin de llenar el arreglo hasta 512*/
//			contad_C++;
//			/*Se evalua si se llega a 512 datos*/
//			if (contad_C == TAMAÑO_DE_DATOS) {
//				/*Se reinicia el contador en caso de que quiera por accion de la tecla "c" volver a recolectar datos*/
//				contad_C = 0;
//				/*Se levanta la bandera para que no se siga recolectando los datos en este arreglo Señal C*/
//				tecla_C = RESET;
//				/*Se levanta la bandera indicando que se terminó la recolección*/
//				bandera_final_C = SET;
//			}
//		}
//	}
///*******************************************************************************************************************************************/
//	/*Se incrementa el contador para llevar la cuenta de la secuencia de conversion ADC*/
//	_Contador_Secuencia++;
//	/*Se evalua si la secuencia ha llegado a 3, el cual es el numero de canales*/
//	if (_Contador_Secuencia >=CANTIDAD_DE_SENSORES){
//		/*Se reinicia el contador para estar al ritmo de la secuencia de conversion ADC*/
//		_Contador_Secuencia = 0;
//	}
}
/*FINISH*//*********************************************************************************************************************************/

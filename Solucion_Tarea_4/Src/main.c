/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Cristhian Andrés Oviedo Montalvo
 * @brief          : Solución de la Tarea 4 de Taller 5
 * Fecha           : 18 de Noviembre 2023
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


#define CANTIDAD_DE_SENSORES 3

/*Se encabezan las funciones a utilizar en el programa*/
void start(void);


/*Se define el Led zul del blinky*/
GPIO_Handler_t led_Blinky = {0};           //PB10

/*Se definen los timer a utilizar*/
Timer_Handler_t timer_del_Blinky = {0};

/*Se define el Usart y sus respectivos pines*/
GPIO_Handler_t Transmisor = {0};   //PA2
GPIO_Handler_t Receptor = {0};     //PA3
USART_Handler_t usart2 = {0};

/*Se define los tres sensores tipo ADC_Config_t*/
ADC_Config_t _Sensor1 = {0};
ADC_Config_t _Sensor2 = {0};
ADC_Config_t _Sensor3 = {0};
/*Se crea un arreglo que va a contener los sensores*/
ADC_Config_t _Sensores[CANTIDAD_DE_SENSORES]= {0};

/*Se crea el PWM que va ayudar a muestrear las señales*/
Pwm_Handler_t _PWM_Muestreo = {0};

/*variable que guarda el caracter recibido tras presionar una tecla*/
uint8_t teclado = 0;
/*Se crea una variable para guardar las conversiones que se hace en cada canal*/
uint8_t _Contador_Secuencia = 0;
/**/
uint16_t contador1 = 0;
uint16_t contador2 = 0;
uint16_t contador3 = 0;

uint8_t recoleccion1 = 0;
uint8_t recoleccion2 = 0;
uint8_t recoleccion3 = 0;

uint16_t _Valores_Sensor1[512] = {0};
uint16_t _Valores_Sensor2[512] = {0};
uint16_t _Valores_Sensor3[512] = {0};


/*Definimos el arreglo en el que se guardará el mensaje para cool Term*/
char buffer_info[128] = {0};


int main(void) {

	/*Se activa el co-procesador FPU*/
	SCB->CPACR |= (0b11 << 20);

	/*funcion que se encarga de configurar las definiciones antes mencionadas*/
	start();

	/*Lo que realiza el codigo ciclicamente*/
	while (1) {

//		if (adcComplete) {
//			adcComplete = 0;
//	        sprintf(buffer_info, "Sensor 1 = %d  = %.2f[V]\n\rSensor 2 = %d  = %.2f[V]\n\rSensor 3 = %d  = %.2f[V]\n\r", _Sensores[0].adcData, _Sensores[0].adcData * (float)(3.3 / 4095),_Sensores[1].adcData, _Sensores[1].adcData * (float)(3.3 / 4095),_Sensores[2].adcData, _Sensores[2].adcData * (float)(3.3 / 4095));
//			usart_writeMsg(&usart2, buffer_info);
//		}
		/*Si lo que recibe la variable teclado es diferente del caracter nulo entonces ocurrirá lo que sigue*/
		if (teclado != '\0') {
			/*Si se presiona la letra "p" entonces se muestra un menu de ayuda para los comandos que se pueden utilizar */
			if (teclado == 'p') {
				usart_writeMsg(&usart2, "CALCULADOR DE FRECUENCIAS\n\r");
				recoleccion1 = SET;
				recoleccion2 = SET;
				recoleccion3 = SET;
				teclado = 0;
			}
		}
	}
}

void start(void) {
	/*Se configura el led azul del blinky*/
	led_Blinky.pGPIOx = GPIOB;
	led_Blinky.pinConfig.GPIO_PinNumber = PIN_10;
	led_Blinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	led_Blinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	led_Blinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	led_Blinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&led_Blinky);
	/*Se configura el timer para el blinky*/
	timer_del_Blinky.pTIMx = TIM3;
	timer_del_Blinky.TIMx_Config.TIMx_Prescaler = 16000;
	timer_del_Blinky.TIMx_Config.TIMx_Period = 250;
	timer_del_Blinky.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	timer_del_Blinky.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	timer_Config(&timer_del_Blinky);
	/*que el timer esté encendido*/
	timer_SetState(&timer_del_Blinky, TIMER_ON);
	/*Se congura el usart de comunicacion serial*/
	usart2.ptrUSARTx = USART2;
	usart2.USART_Config.baudrate = USART_BAUDRATE_115200;
	usart2.USART_Config.datasize = USART_DATASIZE_8BIT;
	usart2.USART_Config.parity = USART_PARITY_NONE;
	usart2.USART_Config.stopbits = USART_STOPBIT_1;
	usart2.USART_Config.mode = USART_MODE_RXTX;
	usart2.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart_Config(&usart2);
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
	_Sensor1.channel = CHANNEL_5;
	_Sensor1.resolution = RESOLUTION_12_BIT;
	_Sensor1.dataAlignment = ALIGNMENT_RIGHT;
	_Sensor1.samplingPeriod = SAMPLING_PERIOD_480_CYCLES;
	_Sensor1.interrupState = ADC_INT_ENABLE;

	/*El sensor 2 siendo el segundo de la secuencia*/
	_Sensor2.channel = CHANNEL_6;
	_Sensor2.resolution = RESOLUTION_12_BIT;
	_Sensor2.dataAlignment = ALIGNMENT_RIGHT;
	_Sensor2.samplingPeriod = SAMPLING_PERIOD_480_CYCLES;
	_Sensor2.interrupState = ADC_INT_ENABLE;

	/*El sensor 3 siendo el tercero de la secuencia*/
	_Sensor3.channel = CHANNEL_7;
	_Sensor3.resolution = RESOLUTION_12_BIT;
	_Sensor3.dataAlignment = ALIGNMENT_RIGHT;
	_Sensor3.samplingPeriod = SAMPLING_PERIOD_480_CYCLES;
	_Sensor3.interrupState = ADC_INT_ENABLE;


	/*se asigna la posicion en el arreglo segun la secuencia */
	_Sensores[0] = _Sensor1;
	_Sensores[1] = _Sensor2;
	_Sensores[2] = _Sensor3;

	/*Ahora que se cargue la configuracion de todos los sensores*/
	adc_ConfigMultichannel (_Sensores,CANTIDAD_DE_SENSORES);

	/*Se configura el PWM que va a ayudar a muestrear la señal */
	_PWM_Muestreo.ptTIMx = TIM5;
	_PWM_Muestreo.config.Canal = channel_2_Pwm;
	_PWM_Muestreo.config.prescaler = 16;
	_PWM_Muestreo.config.periodo = 25;
	_PWM_Muestreo.config.CicloDuty = 5;
	configuracion_del_pwm(&_PWM_Muestreo);
	inicio_de_señal_pwm(&_PWM_Muestreo);

	/*Se configura el trigger externo*/
	adc_ConfigTrigger (TRIGGER_EXT, &_PWM_Muestreo);
}

/*Con esta funcion de interrupcion se cambia el estado del led dependiendo del ARR*/
void Timer3_Callback(void) {
	gpio_TooglePin(&led_Blinky);
}
/*Funcion que si se recibe algo por comunicacion serial almacena la informacion en la variable teclado*/
void usart2_RxCallback(void) {
	teclado = usart_getRxData();
}
/* */
void adc_CompleteCallback(void) {
	/**/
//	_Sensores[_Contador_Secuencia].adcData = adc_GetValue();
//	_Contador_Secuencia++; /**/
//
//	if (_Contador_Secuencia >= CANTIDAD_DE_SENSORES){
//		_Contador_Secuencia = 0; /**/
//	}

	_Sensores[_Contador_Secuencia].adcData = adc_GetValue();
	if(_Contador_Secuencia ==0 && recoleccion1 == SET){
		_Valores_Sensor1[contador1]=_Sensores[_Contador_Secuencia].adcData ;
		contador1++;
		if(contador1 >= 512){
			recoleccion1 = RESET;
		}
	}

	if(_Contador_Secuencia ==1 && recoleccion2 == SET){
		_Valores_Sensor2[contador2]=_Sensores[_Contador_Secuencia].adcData ;
		contador2++;
		if(contador2 >= 512){
			recoleccion2 = RESET;
		}
	}

	if(_Contador_Secuencia ==2 && recoleccion3 == SET){
		_Valores_Sensor3[contador3]=_Sensores[_Contador_Secuencia].adcData ;
		contador3++;
		if(contador3 >= 512){
			recoleccion3 = RESET;
		}
	}

	_Contador_Secuencia++; /**/

	if (_Contador_Secuencia >= CANTIDAD_DE_SENSORES){
		_Contador_Secuencia = 0; /**/
	}
}








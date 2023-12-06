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
#define CANTIDAD_DE_SENSORES 3
/*Se encabeza la funcion a utilizar en el programa*/
void start(void);
/*Se define el Led rojo del blinky*/
GPIO_Handler_t led_Blinky = {0};         //PB10
/*Se define el timer a utilizar*/
Timer_Handler_t timer_del_Blinky = {0};
/*Se define el Usart y sus respectivos pines*/
USART_Handler_t emisor = { 0 };
GPIO_Handler_t pinTx = { 0 };
GPIO_Handler_t pinRx = { 0 };
uint8_t RxData = 0;/*variable que guarda el caracter recibido tras presionar una tecla*/
/*Definimos el arreglo en el que se guardará el mensaje para cool Term*/
char buffer_info[128] = {0};

uint16_t _Duty = 800;
PWM_Handler_t _PWM_ENA = {0};
GPIO_Handler_t _ENA = {0};
GPIO_Handler_t _IN1 = {0};
GPIO_Handler_t _IN2 = {0};

uint16_t _Duty2 = 800;
PWM_Handler_t _PWM_ENB = {0};
GPIO_Handler_t _ENB = {0};
GPIO_Handler_t _IN3 = {0};
GPIO_Handler_t _IN4 = {0};

PWM_Handler_t _PWM_Muestreo = {0};
ADC_Config_t arreglo_ejes[CANTIDAD_DE_SENSORES]= {0};
ADC_Config_t ejeX = {0};
ADC_Config_t ejeY = {0};
ADC_Config_t ejeYP = {0};
uint16_t Lectura_ejeX = 0;
uint16_t Lectura_ejeY = 0;
uint16_t Lectura_ejeYP = 0;
uint8_t _Contador_Secuencia = 0; /*Contador de la secuencia de conversion*/

uint8_t flag_boton = 0;
EXTI_Config_t _Exti1Botton = {0};
GPIO_Handler_t _SW_cambiaPWM = {0};	//PIN A1
uint8_t sendMsg = 0;

///*Pines para el RGB*/
//GPIO_Handler_t RGB_rojo = {0};
//GPIO_Handler_t RGB_verde  = {0};
//GPIO_Handler_t RGB_azul  = {0};



int main(void) {


	/*funcion que se encarga de configurar las definiciones antes mencionadas*/
	start();

	/*Lo que realiza el codigo ciclicamente*/
	while (1) {


		if (flag_boton ==  0){

			if (Lectura_ejeYP > 1800 && Lectura_ejeYP < 2300) {
				usart_WriteChar(&emisor,'m');
			}
//			/*adelante*/
	        if (Lectura_ejeYP < 300) {
				usart_WriteChar(&emisor,'h');
			}
			/*atras*/
			if (Lectura_ejeYP > 3700) {
				usart_WriteChar(&emisor,'l');
			}

				/*Quieto*/
			if (Lectura_ejeY > 1800 && Lectura_ejeY < 2300 && Lectura_ejeX > 1800 && Lectura_ejeX < 2300) {
				usart_WriteChar(&emisor,'q');
			}
//			/*adelante*/
	        if (Lectura_ejeY < 300) {
				usart_WriteChar(&emisor,'a');
			}
			/*atras*/
			else if (Lectura_ejeY > 3700) {
				usart_WriteChar(&emisor,'t');
			}
			/*derecha*/
			else if (Lectura_ejeX < 300) {
				usart_WriteChar(&emisor,'d');
			}
//			/*izquierda*/
			else if (Lectura_ejeX > 3700) {
				usart_WriteChar(&emisor,'i');
			}


		}
		else {
			usart_WriteChar(&emisor,'u');

		}
	}


}
/*******************************************************************************************************************************************/
void start(void) {
	/*Configuracion del _SW_cambiaPWM
	 Boton usado para cambiar el sentido del contador */
	_SW_cambiaPWM.pGPIOx = GPIOB;
	_SW_cambiaPWM.pinConfig.GPIO_PinNumber = PIN_9;
	_SW_cambiaPWM.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	_SW_cambiaPWM.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_SW_cambiaPWM.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_SW_cambiaPWM.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	_Exti1Botton.pGPIOHandler = &_SW_cambiaPWM;
	_Exti1Botton.edgeType = EXTERNAL_INTERRUPT_RISING_FALLING_EDGE;
	exti_Config(&_Exti1Botton);
	/*Se configura el led rojo del blinky*/
	led_Blinky.pGPIOx = GPIOC;
	led_Blinky.pinConfig.GPIO_PinNumber = PIN_1;
	led_Blinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	led_Blinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	led_Blinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	led_Blinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&led_Blinky);
	gpio_WritePin(&led_Blinky, SET);
	/*Se configura el timer para el blinky*/
	timer_del_Blinky.pTIMx = TIM9;
	timer_del_Blinky.TIMx_Config.TIMx_Prescaler = 16000;
	timer_del_Blinky.TIMx_Config.TIMx_Period = 250;
	timer_del_Blinky.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	timer_del_Blinky.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	timer_Config(&timer_del_Blinky);
	/*Que el timer esté encendido*/
	timer_SetState(&timer_del_Blinky, TIMER_ON);
	/*Se configura el usart de comunicacion serial*/
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
	emisor.ptrUSARTx = USART6;
	emisor.USART_Config.baudrate = USART_BAUDRATE_9600;
	emisor.USART_Config.datasize = USART_DATASIZE_8BIT;
	emisor.USART_Config.parity = USART_PARITY_NONE;
	emisor.USART_Config.stopbits = USART_STOPBIT_1;
	emisor.USART_Config.mode = USART_MODE_RXTX;
	emisor.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart_Config(&emisor);

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
	_PWM_ENB.config.CicloDuty = 1000;
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
	gpio_WritePin(&_IN4,SET);

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
	gpio_WritePin(&_IN2,SET);

	_IN1.pGPIOx = GPIOB;
	_IN1.pinConfig.GPIO_PinNumber = PIN_6;
	_IN1.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_IN1.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_IN1.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_IN1.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_IN1);
	gpio_WritePin(&_IN1,SET);

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
	_PWM_ENA.config.CicloDuty = 1000;
	configuracion_del_pwm(&_PWM_ENA);
	activar_salida(&_PWM_ENA);
	inicio_de_señal_pwm(&_PWM_ENA);
	/*El sensor 1 siendo el primero de la secuencia*/
	ejeX.channel = CHANNEL_7;
	ejeX.resolution = RESOLUTION_12_BIT;
	ejeX.dataAlignment = ALIGNMENT_RIGHT;
	ejeX.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
	ejeX.interrupState = ADC_INT_ENABLE;
	/*El sensor 2 siendo el segundo de la secuencia*/
	ejeY.channel = CHANNEL_6;
	ejeY.resolution = RESOLUTION_12_BIT;
	ejeY.dataAlignment = ALIGNMENT_RIGHT;
	ejeY.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
	ejeY.interrupState = ADC_INT_ENABLE;

	/*El sensor 2 siendo el segundo de la secuencia*/
	ejeYP.channel = CHANNEL_14;
	ejeYP.resolution = RESOLUTION_12_BIT;
	ejeYP.dataAlignment = ALIGNMENT_RIGHT;
	ejeYP.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
	ejeYP.interrupState = ADC_INT_ENABLE;

	arreglo_ejes[0] = ejeX;
	arreglo_ejes[1] = ejeY;
	arreglo_ejes[2] = ejeYP;

	/*Ahora que se cargue la configuracion de todos los sensores*/
	adc_ConfigMultichannel (arreglo_ejes,CANTIDAD_DE_SENSORES);
	/*Se configura el PWM que va a ayudar a muestrear la señal */
	_PWM_Muestreo.pTIMx = TIM4;
	_PWM_Muestreo.config.Canal = PWM_CHANNEL_4;
	_PWM_Muestreo.config.prescaler = 16;
	_PWM_Muestreo.config.periodo = 100;
	_PWM_Muestreo.config.CicloDuty = 50;
	configuracion_del_pwm(&_PWM_Muestreo);
	inicio_de_señal_pwm(&_PWM_Muestreo);
	inicio_de_señal_pwm(&_PWM_Muestreo);
	/*Se configura el trigger externo con PWM*/
	adc_ConfigTrigger (TRIGGER_EXT, &_PWM_Muestreo);

//	RGB_rojo.pGPIOx = GPIOB;
//	RGB_rojo.pinConfig.GPIO_PinNumber = PIN_10;
//	RGB_rojo.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
//	RGB_rojo.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
//	RGB_rojo.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
//	RGB_rojo.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	gpio_Config(&RGB_rojo);
//
//	RGB_verde.pGPIOx = GPIOB;
//	RGB_verde.pinConfig.GPIO_PinNumber = PIN_10;
//	RGB_verde.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
//	RGB_verde.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
//	RGB_verde.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
//	RGB_verde.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	gpio_Config(&RGB_verde);
//
//	RGB_azul.pGPIOx = GPIOB;
//	RGB_azul.pinConfig.GPIO_PinNumber = PIN_10;
//	RGB_azul.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
//	RGB_azul.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
//	RGB_azul.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
//	RGB_azul.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	gpio_Config(&RGB_azul);


}


/*******************************************************************************************************************************************/

/*Con esta funcion de interrupcion se cambia el estado del led dependiendo del ARR*/
void Timer9_Callback(void) {
	gpio_TooglePin(&led_Blinky);
//	sendMsg = 1;
}
/*Funcion que si se recibe algo por comunicacion serial almacena la informacion en la variable teclado*/
void usart6_RxCallback(void){
	//leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto además debe bajar la bandera de la interrupción.
	RxData = usart_getRxData();

}

void callback_extInt9(void) {
	flag_boton ^= 1;
}
/*Esta funcion se activa por cada conversion segun la secuencia*/
void adc_CompleteCallback(void) {
	arreglo_ejes[_Contador_Secuencia].adcData = adc_GetValue();
	if (_Contador_Secuencia == 0){
		Lectura_ejeX = arreglo_ejes[0].adcData;
	}
	else if (_Contador_Secuencia == 1){
		Lectura_ejeY = arreglo_ejes[1].adcData;
	}
	else if (_Contador_Secuencia == 2){
		Lectura_ejeYP = arreglo_ejes[2].adcData;
	}
	_Contador_Secuencia++;
	/*Se evalua si la secuencia ha llegado a 3, el cual es el numero de canales*/
	if (_Contador_Secuencia >=CANTIDAD_DE_SENSORES){
		/*Se reinicia el contador para estar al ritmo de la secuencia de conversion ADC*/
		_Contador_Secuencia = 0;
	}
}
/*FINISH*//*********************************************************************************************************************************/




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
/*******************************************************************************************************************************************/

/*Se hace los #define para calcular la frecuencia*/
#define TAMAÑO_DE_DATOS 512
#define CANTIDAD_DE_SENSORES 3
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
USART_Handler_t usart2 = {0};
/*Definimos el arreglo en el que se guardará el mensaje para cool Term*/
char buffer_info[128] = {0};
/*Se define los tres sensores tipo ADC_Config_t*/
ADC_Config_t _Sensor1 = {0};  //PB1
ADC_Config_t _Sensor2 = {0};  //PB0
ADC_Config_t _Sensor3 = {0};  //PA6
/*Se crea un arreglo que va a contener los sensores*/
ADC_Config_t _Sensores[CANTIDAD_DE_SENSORES]= {0};
/*Se crea el PWM que va ayudar a muestrear las señales*/
PWM_Handler_t _PWM_Muestreo = {0};

/*******************************************************************************************************************************************/
/*Se definen los elementos necesarios para encontrar el valor maximo y minimo de la señal del sensor "A" y su frecuencia*/
float32_t frecuencia_A = 0;  /*Frecuencia de la señal*/
uint32_t indice_A = 0; /*Indice de la posicion de mayor valor en el arreglo de la señal original*/
uint32_t indice_max_A =0; /*Indice de la posicion de la frecuencia dominante */
float32_t maxValue_A; /*Máximo valor de la señal original*/
float32_t minValue_A; /*Mínimo valor de la señal original*/
float32_t maxValue_Afft; /*maximo valor de la transformada de fourier*/
float32_t señal_A[TAMAÑO_DE_DATOS]; /*Arreglo de datos despues de muestrear la señal con el PWM*/
float32_t transformedSignal_A[TAMAÑO_DE_DATOS]={0}; /*Arreglo de datos despues de aplicar la transformada de fourier a la señal original*/
float32_t señal_mejorada_A[TAMAÑO_DE_DATOS]={0}; /*Arreglo de datos de la transformada pero sin la parte compleja*/
float32_t señal_final_A[TAMAÑO_DE_DATOS] = {0}; /*Aqui ya se encuentran los de la señal mejorada pero en valor absoluto*/
/*******************************************************************************************************************************************/
/*Se hace la definicion para encontrar el valor maximo y minimo de la señal del sensor "B" y su frecuencia*/
float32_t frecuencia_B = 0; /*Frecuencia de la señal*/
uint32_t indice_B = 0; /*Indice de la posicion de mayor valor en el arreglo de la señal original*/
uint32_t indice_max_B =0; /*Indice de la posicion de la frecuencia dominante */
float32_t maxValue_B; /*Máximo valor de la señal original*/
float32_t minValue_B; /*Mínimo valor de la señal original*/
float32_t maxValue_Bfft; /*maximo valor de la transformada de fourier*/
float32_t señal_B[TAMAÑO_DE_DATOS]; /*Arreglo de datos despues de muestrear la señal con el PWM*/
float32_t transformedSignal_B[TAMAÑO_DE_DATOS]={0}; /*Arreglo de datos despues de aplicar la transformada de fourier a la señal original*/
float32_t señal_mejorada_B[TAMAÑO_DE_DATOS]={0}; /*Arreglo de datos de la transformada pero sin la parte compleja*/
float32_t señal_final_B[TAMAÑO_DE_DATOS] = {0}; /*Aqui ya se encuentran los de la señal mejorada pero en valor absoluto*/
/*******************************************************************************************************************************************/
/*Se define para encontrar el maximo y minimo de la señal del sensor "C" y su frecuencia*/
float32_t frecuencia_C = 0; /*Frecuencia de la señal*/
uint32_t indice_C = 0; /*Indice de la posicion de mayor valor en el arreglo de la señal original*/
uint32_t indice_max_C =0; /*Indice de la posicion de la frecuencia dominante */
uint32_t ifftFlag = 0; /*indicador de transformada*/
float32_t maxValue_C; /*Máximo valor de la señal original*/
float32_t minValue_C; /*Mínimo valor de la señal original*/
float32_t maxValue_Cfft; /*maximo valor de la transformada de fourier*/
float32_t señal_C[TAMAÑO_DE_DATOS]; /*Arreglo de datos despues de muestrear la señal con el PWM*/
float32_t transformedSignal_C[TAMAÑO_DE_DATOS] = {0}; /*Arreglo de datos despues de aplicar la transformada de fourier a la señal original*/
float32_t señal_mejorada_C[TAMAÑO_DE_DATOS] = {0}; /*Arreglo de datos de la transformada pero sin la parte compleja*/
float32_t señal_final_C[TAMAÑO_DE_DATOS] = {0}; /*Aqui ya se encuentran los de la señal mejorada pero en valor absoluto*/
/*******************************************************************************************************************************************/
/**/
uint16_t tecla_A = 0;
uint16_t bandera_final_A = 0;
uint16_t contad_A = 0;

uint16_t tecla_B = 0;
uint16_t bandera_final_B = 0;
uint16_t contad_B = 0;

uint16_t tecla_C = 0;
uint16_t bandera_final_C = 0;
uint16_t contad_C = 0;


arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_status status =ARM_MATH_ARGUMENT_ERROR;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;

/*variable que guarda el caracter recibido tras presionar una tecla*/
uint8_t teclado = 0;
/*Se crea una variable para guardar las conversiones que se hace en cada canal*/
uint8_t _Contador_Secuencia = 0;


int main(void) {
	/*Se activa el co-procesador FPU*/
	SCB->CPACR |= (0xF << 20);
	/*funcion que se encarga de configurar las definiciones antes mencionadas*/
	start();
	/*Lo que realiza el codigo ciclicamente*/
	while (1) {

		if (bandera_final_A) {
			bandera_final_A = RESET;
			frenar_señal_pwm (&_PWM_Muestreo);
			arm_absmax_f32 (señal_A, TAMAÑO_DE_DATOS, &maxValue_A, &indice_A);
			arm_min_f32(señal_A, TAMAÑO_DE_DATOS, &minValue_A, &indice_A);
			statusInitFFT=arm_rfft_fast_init_f32(&config_Rfft_fast_f32,TAMAÑO_DE_DATOS);
			if(statusInitFFT == ARM_MATH_SUCCESS){
			arm_rfft_fast_f32(&config_Rfft_fast_f32, señal_A,transformedSignal_A, ifftFlag);
			arm_cmplx_mag_f32(transformedSignal_A,señal_mejorada_A,TAMAÑO_DE_DATOS/2);
			arm_abs_f32(señal_mejorada_A,señal_final_A,TAMAÑO_DE_DATOS/2);
			señal_final_A[0] = 0;
			arm_max_f32(señal_final_A, TAMAÑO_DE_DATOS/2, &maxValue_Afft, &indice_max_A);
			frecuencia_A = ((indice_max_A*FRECUENCIA_DE_MUESTREO)/TAMAÑO_DE_DATOS)/2;
			usart_writeMsg(&usart2, "Sensor 1 __________________________\n\r");
			sprintf(buffer_info, "Frecuencia = %.f [Hz]\n\rMáximo de la señal = %.f = %.1f [V]\n\rMínimo de la señal = %.f = %.1f [V] \n\r", frecuencia_A,maxValue_A,maxValue_A*(float)(3.3 / 4095),minValue_A,minValue_A*(float)(3.3 / 4095));
			usart_writeMsg(&usart2, buffer_info);
			}
		}

		if (bandera_final_B) {
			bandera_final_B = RESET;
			frenar_señal_pwm (&_PWM_Muestreo);
			arm_absmax_f32 (señal_B, TAMAÑO_DE_DATOS, &maxValue_B, &indice_B);
			arm_min_f32(señal_B, TAMAÑO_DE_DATOS, &minValue_B, &indice_B);
			statusInitFFT=arm_rfft_fast_init_f32(&config_Rfft_fast_f32,TAMAÑO_DE_DATOS);
			if(statusInitFFT == ARM_MATH_SUCCESS){
			arm_rfft_fast_f32(&config_Rfft_fast_f32, señal_B,transformedSignal_B, ifftFlag);
			arm_cmplx_mag_f32(transformedSignal_B,señal_mejorada_B,TAMAÑO_DE_DATOS/2);
			arm_abs_f32(señal_mejorada_B,señal_final_B,TAMAÑO_DE_DATOS/2);
			señal_final_B[0] = 0;
			arm_max_f32(señal_final_B, TAMAÑO_DE_DATOS/2, &maxValue_Bfft, &indice_max_B);
			frecuencia_B = ((indice_max_B*FRECUENCIA_DE_MUESTREO)/TAMAÑO_DE_DATOS)/2;
			usart_writeMsg(&usart2, "Sensor 2 __________________________\n\r");
			sprintf(buffer_info, "Frecuencia = %.f [Hz]\n\rMáximo de la señal = %.f = %.1f [V]\n\rMínimo de la señal = %.f = %.1f [V] \n\r", frecuencia_B,maxValue_B,maxValue_B*(float)(3.3 / 4095),minValue_B,minValue_B*(float)(3.3 / 4095));
			usart_writeMsg(&usart2, buffer_info);
			}
		}
		if (bandera_final_C) {
			bandera_final_C = RESET;
			frenar_señal_pwm (&_PWM_Muestreo);
			arm_absmax_f32 (señal_C, TAMAÑO_DE_DATOS, &maxValue_C, &indice_C);
			arm_min_f32(señal_C, TAMAÑO_DE_DATOS, &minValue_C, &indice_C);
			statusInitFFT=arm_rfft_fast_init_f32(&config_Rfft_fast_f32,TAMAÑO_DE_DATOS);
			if(statusInitFFT == ARM_MATH_SUCCESS){
			arm_rfft_fast_f32(&config_Rfft_fast_f32, señal_C,transformedSignal_C, ifftFlag);
			arm_cmplx_mag_f32(transformedSignal_C,señal_mejorada_C,TAMAÑO_DE_DATOS/2);
			arm_abs_f32(señal_mejorada_C,señal_final_C,TAMAÑO_DE_DATOS/2);
			señal_final_C[0] = 0;
			arm_max_f32(señal_final_C, TAMAÑO_DE_DATOS/2, &maxValue_Cfft, &indice_max_C);
			frecuencia_C = ((indice_max_C*FRECUENCIA_DE_MUESTREO)/TAMAÑO_DE_DATOS)/2;
			usart_writeMsg(&usart2, "Sensor 3 __________________________\n\r");
			sprintf(buffer_info, "Frecuencia = %.f [Hz]\n\rMáximo de la señal = %.f = %.1f [V]\n\rMínimo de la señal = %.f = %.1f [V] \n\r", frecuencia_C,maxValue_C,maxValue_C*(float)(3.3 / 4095),minValue_C,minValue_C*(float)(3.3 / 4095));
			usart_writeMsg(&usart2, buffer_info);
			}
		}
		if (teclado != '\0') {

			if (teclado == 'a') {
				inicio_de_señal_pwm(&_PWM_Muestreo);
				tecla_A = SET;
				teclado = 0;
			}
			if (teclado == 'b') {
				inicio_de_señal_pwm(&_PWM_Muestreo);
				tecla_B = SET;
				teclado = 0;
			}
			if (teclado == 'c') {
				inicio_de_señal_pwm(&_PWM_Muestreo);
				tecla_C = SET;
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
	timer_del_Blinky.pTIMx = TIM9;
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
	_Sensor1.channel = CHANNEL_9;
	_Sensor1.resolution = RESOLUTION_12_BIT;
	_Sensor1.dataAlignment = ALIGNMENT_RIGHT;
	_Sensor1.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
	_Sensor1.interrupState = ADC_INT_ENABLE;
	/*El sensor 2 siendo el segundo de la secuencia*/
	_Sensor2.channel = CHANNEL_8;
	_Sensor2.resolution = RESOLUTION_12_BIT;
	_Sensor2.dataAlignment = ALIGNMENT_RIGHT;
	_Sensor2.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
	_Sensor2.interrupState = ADC_INT_ENABLE;
	/*El sensor 3 siendo el tercero de la secuencia*/
	_Sensor3.channel = CHANNEL_6;
	_Sensor3.resolution = RESOLUTION_12_BIT;
	_Sensor3.dataAlignment = ALIGNMENT_RIGHT;
	_Sensor3.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
	_Sensor3.interrupState = ADC_INT_ENABLE;
	/*se asigna la posicion en el arreglo segun la secuencia */
	_Sensores[0] = _Sensor1;
	_Sensores[1] = _Sensor2;
	_Sensores[2] = _Sensor3;
	/*Ahora que se cargue la configuracion de todos los sensores*/
	adc_ConfigMultichannel (_Sensores,CANTIDAD_DE_SENSORES);
	/*Se configura el PWM que va a ayudar a muestrear la señal */
	_PWM_Muestreo.pTIMx = TIM4;
	_PWM_Muestreo.config.Canal = PWM_CHANNEL_4;
	_PWM_Muestreo.config.prescaler = 16;
	_PWM_Muestreo.config.periodo = 25;
	_PWM_Muestreo.config.CicloDuty = 2;
	configuracion_del_pwm(&_PWM_Muestreo);
	inicio_de_señal_pwm(&_PWM_Muestreo);

	/*Se configura el trigger externo*/
	adc_ConfigTrigger (TRIGGER_EXT, &_PWM_Muestreo);
}
/*Con esta funcion de interrupcion se cambia el estado del led dependiendo del ARR*/
void Timer9_Callback(void) {
	gpio_TooglePin(&led_Blinky);
}
/*Funcion que si se recibe algo por comunicacion serial almacena la informacion en la variable teclado*/
void usart2_RxCallback(void) {
	teclado = usart_getRxData();
}
/* */
void adc_CompleteCallback(void) {

	_Sensores[_Contador_Secuencia].adcData = adc_GetValue();

	if (tecla_A) {
		if (_Contador_Secuencia == 0) {
			señal_A[contad_A] = _Sensores[0].adcData;
			contad_A++;
			if (contad_A == TAMAÑO_DE_DATOS) {
				contad_A = 0;
				tecla_A = RESET;
				bandera_final_A = SET;
			}
		}
	}

	if (tecla_B) {
		if (_Contador_Secuencia == 1) {
			señal_B[contad_B] = _Sensores[1].adcData;
			contad_B++;
			if (contad_B == TAMAÑO_DE_DATOS) {
				contad_B = 0;
				tecla_B = RESET;
				bandera_final_B = SET;
			}
		}
	}

	if (tecla_C) {
		if (_Contador_Secuencia == 2) {
			señal_C[contad_C] = _Sensores[2].adcData;
			contad_C++;
			if (contad_C == TAMAÑO_DE_DATOS) {
				contad_C = 0;
				tecla_C = RESET;
				bandera_final_C = SET;
			}
		}
	}
	_Contador_Secuencia++;
	if (_Contador_Secuencia >=CANTIDAD_DE_SENSORES){
		_Contador_Secuencia = 0;
	}
}





















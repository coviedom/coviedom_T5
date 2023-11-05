/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Cristhian Andrés Oviedo Montalvo
 * @brief          : Solución de la Tarea 3 de Taller 5
 * Fecha           : 29 de octubre 2023
 ******************************************************************************
 */
#include <stdint.h>
#include <stm32f4xx.h>
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "exti_driver_hal.h"
#include "timer_driver_hal.h"

/*Se encabezan las funciones a utilizar en el programa*/
void start(void);
void mostrar_resultados_test(uint8_t number_resolucion);
void mostrar_resolucion_elegida(uint8_t num_resolu);
void mostrar_sensor_elegido(uint8_t num_sensor);
void cambiador_de_canal_sensor(ADC_Config_t *_ADC, uint16_t _Canal);
void cambiador_de_resolucion(ADC_Config_t *adc, uint16_t resoluc);
void enviarInfo_al_7_segmentos(void);
void establecer_resolucion_en_pantalla(uint8_t resolucion);
void establecer_sensor_en_pantalla(uint8_t sensor);

/*Se define el Led zul del blinky*/
GPIO_Handler_t led_Blinky = {0};              //PB10

/*Se definen los segmentos del display*/
GPIO_Handler_t puntico = {0};                 //PC6
GPIO_Handler_t segmento_A = {0};   		      //PC12
GPIO_Handler_t segmento_B = {0};   		      //PC11
GPIO_Handler_t segmento_C = {0};   		      //PC5
GPIO_Handler_t segmento_D = {0};   		      //PC9
GPIO_Handler_t segmento_E = {0};   		      //PC8
GPIO_Handler_t segmento_G = {0};   		      //PB9
GPIO_Handler_t transistor_sensores = {0};     //PB6
GPIO_Handler_t transistor_resoluciones = {0}; //PB8

/*Se definen los timer a utilizar*/
Timer_Handler_t timer_del_Blinky = {0};
Timer_Handler_t timer_Display = {0};
Timer_Handler_t timer_refresh = {0}; /*Este es el timer que actualiza los datos del test ADC*/

/*Definicion de los pines e interrupciones para lo relacionado al encoder*/
EXTI_Config_t exti_del_clock = {0};           //Exti 3
GPIO_Handler_t clock_del_encoder = {0};	      //PC3
GPIO_Handler_t _Data = {0};                   //PC1
EXTI_Config_t exti_del_modo = {0};            //Exti 5
GPIO_Handler_t boton_modo = {0};	          //PB5

/*Se define el Usart y sus respectivos pines*/
GPIO_Handler_t Transmisor = {0};
GPIO_Handler_t Receptor = {0};
USART_Handler_t usart1 = {0};

ADC_Config_t _Sensor = {0};

int8_t contador_sensor = 1;
int8_t contador_resolucion = 0;
uint8_t bandera_modo = 0;
uint8_t bandera_giro = 0;
uint8_t bandera_adc = 0;
uint8_t bandera_refresh = 0;
uint8_t teclado = 0;
char buffer_info[128] = {0};

enum{
	Sensor_1 = 1,
	Sensor_2,
	Sensor_3,
};

enum{
	Resolucion_6_Bit = 0,
	Resolucion_8_Bit,
	Resolucion_10_Bit,
	Resolucion_12_Bit
};


int main(void) {
	start();
	while (1) {

		if (teclado == 'j') {
			usart_writeMsg(&usart1, "Test de ADC encendido ! +++++++++++++++++++++++++++++++++++ \n\r");
			timer_SetState(&timer_refresh, TIMER_ON);
			teclado = 0;
		}

		if (bandera_refresh == SET){

			if (bandera_adc) {
				bandera_adc = 0;
				mostrar_resultados_test(contador_resolucion);
			}

			if (teclado != '\0') {

				if (teclado == 'p') {
					usart_writeMsg(&usart1, "Menu de ayuda:\n j -> Enciende el test de ADC  \n k ->  Detiene el test de ADC \n a -> Aumenta \n d -> Disminuye \n m -> Cambia de modo \n Nota: Por defecto el test esta encendido\n \r");
					teclado = 0;
				}

				else if (teclado == 'j') {
					usart_writeMsg(&usart1, "Test de ADC encendido ! +++++++++++++++++++++++++++++++++++ \n\r");
					timer_SetState(&timer_refresh, TIMER_ON);
					teclado = 0;
				}

				else if (teclado == 'k') {
					usart_writeMsg(&usart1, "Se acaba de detener el test de ADC ! ++++++++++++++++++++++\n\r");
					timer_SetState(&timer_refresh, TIMER_OFF);
					bandera_refresh = RESET;
					teclado = 0;
				}

				else if (teclado == 'm'&& bandera_modo == SET ) {
					usart_writeMsg(&usart1, "¡Cambiando a elegir Resoluciones! *************************\n\r");
					bandera_modo ^= 1;
					teclado = 0;
				}
				else if (teclado == 'm'&& bandera_modo == RESET ) {
					usart_writeMsg(&usart1, "¡Cambiando a elegir Sensores! *****************************\n\r");
					bandera_modo ^= 1;
					teclado = 0;
				}

				else if (teclado == 'a') {

					if (bandera_modo == SET) {
						contador_sensor++;

						if (contador_sensor > 3) {
							contador_sensor = 3;
						}
						mostrar_sensor_elegido(contador_sensor);

						teclado = 0;
					}
					else if (bandera_modo == RESET) {

						contador_resolucion++;

						if (contador_resolucion > 3) {
							contador_resolucion = 3;
						}
						mostrar_resolucion_elegida(contador_resolucion);

						teclado = 0;
					}
				}

				if (teclado == 'd') {
					if (bandera_modo == SET) {
						contador_sensor--;
						if (contador_sensor < 1) {
							contador_sensor = 1;
						}
						mostrar_sensor_elegido(contador_sensor);

						teclado = 0;

					}
					else if (bandera_modo == RESET) {
						contador_resolucion--;

						if (contador_resolucion < 0) {
							contador_resolucion = 0;
						}
						mostrar_resolucion_elegida(contador_resolucion);

						teclado = 0;
					}
				}
			}


			if (bandera_giro == SET) {

				bandera_giro = RESET;

				uint32_t _Lectura = gpio_ReadPin(&_Data);

				if (bandera_modo == SET && _Lectura == SET) {

					contador_sensor++;

					if (contador_sensor > 3) {
						contador_sensor = 3;
					}

					mostrar_sensor_elegido(contador_sensor);
				}

				if (bandera_modo == SET && _Lectura == RESET) {

					contador_sensor--;

					if (contador_sensor < 1) {
						contador_sensor = 1;
					}
					mostrar_sensor_elegido(contador_sensor);

				}

				if (bandera_modo == RESET && _Lectura == SET) {

					contador_resolucion++;

					if (contador_resolucion > 3) {
						contador_resolucion = 3;
					}
					mostrar_resolucion_elegida(contador_resolucion);
				}

				if (bandera_modo == RESET && _Lectura == RESET) {
					contador_resolucion--;

					if (contador_resolucion < 0) {
						contador_resolucion = 0;
					}
					mostrar_resolucion_elegida(contador_resolucion);
				}
				enviarInfo_al_7_segmentos();
			}
		}
	}
}

void start(void) {

	puntico.pGPIOx = GPIOC;
	puntico.pinConfig.GPIO_PinNumber = PIN_6;
	puntico.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	puntico.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	puntico.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	puntico.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&puntico);


	segmento_A.pGPIOx = GPIOC;
	segmento_A.pinConfig.GPIO_PinNumber = PIN_12;
	segmento_A.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_A.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_A.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_A.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_A);


	segmento_B.pGPIOx = GPIOC;
	segmento_B.pinConfig.GPIO_PinNumber = PIN_11;
	segmento_B.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_B.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_B.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_B.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_B);


	segmento_C.pGPIOx = GPIOC;
	segmento_C.pinConfig.GPIO_PinNumber = PIN_5;
	segmento_C.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_C.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_C.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_C.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_C);


	segmento_D.pGPIOx = GPIOC;
	segmento_D.pinConfig.GPIO_PinNumber = PIN_9;
	segmento_D.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_D.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_D.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_D.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_D);


	segmento_E.pGPIOx = GPIOC;
	segmento_E.pinConfig.GPIO_PinNumber = PIN_8;
	segmento_E.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_E.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_E.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_E.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_E);


	segmento_G.pGPIOx = GPIOB;
	segmento_G.pinConfig.GPIO_PinNumber = PIN_9;
	segmento_G.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_G.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_G.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_G.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_G);

	led_Blinky.pGPIOx = GPIOB;
	led_Blinky.pinConfig.GPIO_PinNumber = PIN_10;
	led_Blinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	led_Blinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	led_Blinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	led_Blinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&led_Blinky);


	transistor_sensores.pGPIOx = GPIOB;
	transistor_sensores.pinConfig.GPIO_PinNumber = PIN_6;
	transistor_sensores.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	transistor_sensores.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	transistor_sensores.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	transistor_sensores.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&transistor_sensores);


	transistor_resoluciones.pGPIOx = GPIOB;
	transistor_resoluciones.pinConfig.GPIO_PinNumber = PIN_8;
	transistor_resoluciones.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	transistor_resoluciones.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	transistor_resoluciones.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	transistor_resoluciones.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&transistor_resoluciones);


	_Data.pGPIOx = GPIOC;
	_Data.pinConfig.GPIO_PinNumber = PIN_1;
	_Data.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	_Data.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Data.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_Data.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_Data);

	clock_del_encoder.pGPIOx = GPIOC;
	clock_del_encoder.pinConfig.GPIO_PinNumber = PIN_3;
	clock_del_encoder.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	clock_del_encoder.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	clock_del_encoder.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	clock_del_encoder.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	exti_del_clock.pGPIOHandler = &clock_del_encoder;
	exti_del_clock.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	exti_Config(&exti_del_clock);

	boton_modo.pGPIOx = GPIOB;
	boton_modo.pinConfig.GPIO_PinNumber = PIN_5;
	boton_modo.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	boton_modo.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	boton_modo.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	boton_modo.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	exti_del_modo.pGPIOHandler = &boton_modo;
	exti_del_modo.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	exti_Config(&exti_del_modo);

	usart1.ptrUSARTx = USART1;
	usart1.USART_Config.baudrate = USART_BAUDRATE_115200;
	usart1.USART_Config.datasize = USART_DATASIZE_8BIT;
	usart1.USART_Config.parity = USART_PARITY_NONE;
	usart1.USART_Config.stopbits = USART_STOPBIT_1;
	usart1.USART_Config.mode = USART_MODE_RXTX;
	usart1.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart_Config(&usart1);

	Transmisor.pGPIOx = GPIOA;
	Transmisor.pinConfig.GPIO_PinNumber = PIN_9;
	Transmisor.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Transmisor.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Transmisor.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	Transmisor.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Transmisor.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&Transmisor);

	Receptor.pGPIOx = GPIOA;
	Receptor.pinConfig.GPIO_PinNumber = PIN_10;
	Receptor.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Receptor.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Receptor.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	Receptor.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Receptor.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&Receptor);

	_Sensor.channel = CHANNEL_7;
	_Sensor.resolution = RESOLUTION_6_BIT;
	_Sensor.dataAlignment = ALIGNMENT_RIGHT;
	_Sensor.samplingPeriod = SAMPLING_PERIOD_84_CYCLES;
	_Sensor.interrupState = ADC_INT_ENABLE;
	adc_ConfigSingleChannel(&_Sensor);

	timer_del_Blinky.pTIMx = TIM3;
	timer_del_Blinky.TIMx_Config.TIMx_Prescaler = 16000;
	timer_del_Blinky.TIMx_Config.TIMx_Period = 250;
	timer_del_Blinky.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	timer_del_Blinky.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&timer_del_Blinky);
	timer_SetState(&timer_del_Blinky, TIMER_ON);


	timer_Display.pTIMx = TIM5;
	timer_Display.TIMx_Config.TIMx_Prescaler = 16000;
	timer_Display.TIMx_Config.TIMx_Period = 5;
	timer_Display.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	timer_Display.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&timer_Display);
	timer_SetState(&timer_Display, TIMER_ON);

	timer_refresh.pTIMx = TIM4;
	timer_refresh.TIMx_Config.TIMx_Prescaler = 16000;
	timer_refresh.TIMx_Config.TIMx_Period = 5000;
	timer_refresh.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	timer_refresh.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&timer_refresh);
	timer_SetState(&timer_refresh, TIMER_ON);

	gpio_WritePin(&transistor_sensores, SET);
	gpio_WritePin(&transistor_resoluciones, RESET);

}
void mostrar_resultados_test(uint8_t number_resolucion) {

	switch (number_resolucion) {
	    case Resolucion_6_Bit:
	        sprintf(buffer_info, "ADC = %d  = %.2f[V]   | Sensor: %d con Resolución: 6 Bit|\n\r", _Sensor.adcData, _Sensor.adcData * (float)(3.3 / 63), contador_sensor);
	        usart_writeMsg(&usart1, buffer_info);
	        break;
	    case Resolucion_8_Bit:
	        sprintf(buffer_info, "ADC = %d  = %.2f[V]   | Sensor: %d con Resolución: 8 Bit |\n\r", _Sensor.adcData, _Sensor.adcData * (float)(3.3 / 255), contador_sensor);
	        usart_writeMsg(&usart1, buffer_info);
	        break;
	    case Resolucion_10_Bit:
	        sprintf(buffer_info, "ADC = %d  = %.2f[V]   | Sensor: %d con Resolución: 10 Bit|\n\r", _Sensor.adcData, _Sensor.adcData * (float)(3.3 / 1023), contador_sensor);
	        usart_writeMsg(&usart1, buffer_info);
	        break;
	    case Resolucion_12_Bit:
	        sprintf(buffer_info, "ADC = %d  = %.2f[V]   | Sensor: %d con Resolución: 12 Bit |\n\r", _Sensor.adcData, _Sensor.adcData * (float)(3.3 / 4095), contador_sensor);
	        usart_writeMsg(&usart1, buffer_info);
	        break;
	    default:
	        __NOP();
	}

}

void mostrar_resolucion_elegida(uint8_t num_resolu) {
	switch (num_resolu) {
	    case Resolucion_6_Bit:
	        sprintf(buffer_info, "Resolución = 6 Bit   ______________________________________\n\r");
	        usart_writeMsg(&usart1, buffer_info);
	        cambiador_de_resolucion(&_Sensor, RESOLUTION_6_BIT);
	        break;
	    case Resolucion_8_Bit:
	        sprintf(buffer_info, "Resolución = 8 Bit   ______________________________________\n\r");
	        usart_writeMsg(&usart1, buffer_info);
	        cambiador_de_resolucion(&_Sensor, RESOLUTION_8_BIT);
	        break;
	    case Resolucion_10_Bit:
	        sprintf(buffer_info, "Resolución = 10 Bit  ______________________________________\n\r");
	        usart_writeMsg(&usart1, buffer_info);
	        cambiador_de_resolucion(&_Sensor, RESOLUTION_10_BIT);
	        break;
	    case Resolucion_12_Bit:
	        sprintf(buffer_info, "Resolución = 12 Bit  ______________________________________\n\r");
	        usart_writeMsg(&usart1, buffer_info);
	        cambiador_de_resolucion(&_Sensor, RESOLUTION_12_BIT);
	        break;
	    default:
	        __NOP();
	        break;
	}
}

void mostrar_sensor_elegido(uint8_t num_sensor) {

	sprintf(buffer_info, "Sensor = %d  _______________________________________________\n\r", contador_sensor);
	usart_writeMsg(&usart1, buffer_info);

	switch (num_sensor) {
	    case Sensor_1:
	        cambiador_de_canal_sensor(&_Sensor, CHANNEL_7);
	        break;
	    case Sensor_2:
	        cambiador_de_canal_sensor(&_Sensor, CHANNEL_6);
	        break;
	    case Sensor_3:
	        cambiador_de_canal_sensor(&_Sensor, CHANNEL_5);
	        break;
	    default:
	        __NOP();
	        break;
	}
}


void cambiador_de_canal_sensor(ADC_Config_t *_ADC, uint16_t _Canal) {
	_ADC->channel = _Canal;
	adc_ConfigSingleChannel(_ADC);
}

void  cambiador_de_resolucion(ADC_Config_t *adc, uint16_t resoluc) {
	adc->resolution = resoluc;
	adc_ConfigSingleChannel(adc);
}

void enviarInfo_al_7_segmentos(void) {
	uint32_t read1 = gpio_ReadPin(&transistor_sensores);
	if (read1 == SET) {
		establecer_resolucion_en_pantalla(contador_resolucion);
		if (bandera_modo == SET) {
			gpio_WritePin(&puntico, SET);
		}
	}
	uint32_t read2 = gpio_ReadPin(&transistor_resoluciones);
	if (read2 == SET) {
		establecer_sensor_en_pantalla(contador_sensor);
		if (bandera_modo == RESET) {
			gpio_WritePin(&puntico, SET);
		}
	}
}

void establecer_resolucion_en_pantalla(uint8_t resolucion) {

	switch (resolucion) {

	case Resolucion_6_Bit:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, SET);
		gpio_WritePin(&segmento_B, SET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, SET);
		gpio_WritePin(&segmento_E, RESET);
		gpio_WritePin(&segmento_G, SET);
		break;

	case Resolucion_8_Bit:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, SET);
		gpio_WritePin(&segmento_B, SET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, SET);
		break;

	case Resolucion_10_Bit:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, SET);
		gpio_WritePin(&segmento_B, SET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, RESET);
		break;

	case Resolucion_12_Bit:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, RESET);
		gpio_WritePin(&segmento_B, SET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, RESET);
		break;

	default: {
		__NOP();

		break;
	}
	}
}

void establecer_sensor_en_pantalla(uint8_t sensor) {

	switch (sensor) {

	case Sensor_1:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, SET);
		gpio_WritePin(&segmento_B, RESET);
		gpio_WritePin(&segmento_C, RESET);
		gpio_WritePin(&segmento_D, SET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, SET);
		break;

	case Sensor_2:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, RESET);
		gpio_WritePin(&segmento_B, RESET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, RESET);
		gpio_WritePin(&segmento_G, RESET);
		break;

	case Sensor_3:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, RESET);
		gpio_WritePin(&segmento_B, RESET);
		gpio_WritePin(&segmento_C, RESET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, RESET);
		break;

	default: {
		__NOP();
		break;
	}
	}
}
void adc_CompleteCallback(void) {
	bandera_adc = SET;
	_Sensor.adcData = adc_GetValue();
}

void usart1_RxCallback(void) {
	teclado = usart_getRxData();
}
void Timer3_Callback(void) {
	gpio_TooglePin(&led_Blinky);
}
void Timer5_Callback(void) {

	gpio_TooglePin(&transistor_sensores);
	gpio_TooglePin(&transistor_resoluciones);
	enviarInfo_al_7_segmentos();
}

void Timer4_Callback(void) {
	bandera_refresh = SET;
	adc_StartSingleConv();
}

void callback_extInt5(void) {
	bandera_modo ^= 1;
	if (bandera_modo == SET ) {
		usart_writeMsg(&usart1, "¡Cambiando a elegir Sensores! *****************************\n\r");
	}
	if (bandera_modo == RESET ) {
		usart_writeMsg(&usart1, "¡Cambiando a elegir Resoluciones! *************************\n\r");
	}

}

void callback_extInt3(void) {
	bandera_giro = SET;
}


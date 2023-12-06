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
#define CANTIDAD_DE_SENSORES 2
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

//PWM_Handler_t _PWM_Muestreo = {0};
//ADC_Config_t arreglo_ejes[CANTIDAD_DE_SENSORES]= {0};
//ADC_Config_t ejeX = {0};
//ADC_Config_t ejeY = {0};
//uint16_t Lectura_ejeX = 0;
//uint16_t Lectura_ejeY = 0;
//uint8_t _Contador_Secuencia = 0; /*Contador de la secuencia de conversion*/

uint8_t flag_boton = 0;
EXTI_Config_t _Exti1Botton = {0};
GPIO_Handler_t _SW_cambiaPWM = {0};	//PIN A1

GPIO_Handler_t infrarojo_delantero = {0};

///*Pines para el RGB*/
//GPIO_Handler_t RGB_rojo = {0};
//GPIO_Handler_t RGB_verde  = {0};
//GPIO_Handler_t RGB_azul  = {0};



int main(void) {
	/*Se activa el co-procesador FPU*/
//	SCB->CPACR |= (0xF << 20);
	/*funcion que se encarga de configurar las definiciones antes mencionadas*/
	start();

	/*Lo que realiza el codigo ciclicamente*/
	while (1) {


//		if(teclado != '\0') {
//			if (teclado == 'j' ) {
////				sprintf(bufferData, "El Usart Funciona Bien!! %d \n\r",700);
////				usart_writeMsg(&coolterm, bufferData);
//				gpio_TooglePin(&led_Blinky);
//
////				teclado = 0;
////				sendMsg = 1;
//			}
		uint32_t read1 = gpio_ReadPin(&infrarojo_delantero);
		/*Entonces si está en 1 es por que esta encendido el de las resoluciones ya que es de catodo comun*/
//		if (read1 == RESET) {
//			gpio_WritePin(&userLed, SET);
//		}
//		else {
//			gpio_WritePin(&userLed, RESET);
//		}

		if (read1 == SET) {
			gpio_WritePin(&led_Blinky,RESET);


			while (teclado == 'q' ) {

				/*Quieto*/
				gpio_WritePin(&_IN1, RESET);
				gpio_WritePin(&_IN2, RESET);
				gpio_WritePin(&_IN3, RESET);
				gpio_WritePin(&_IN4, RESET);
				teclado = 0;


			}
			while(teclado == 'a' ) {
			/*adelante*/
				gpio_ReadPin(&infrarojo_delantero);
				gpio_WritePin(&_IN1, RESET);
				gpio_WritePin(&_IN2, SET);
				gpio_WritePin(&_IN3, RESET);
				gpio_WritePin(&_IN4, SET);
				teclado = 0;




			}
//
//
//
//
			while (teclado == 't' ) {
			/*atras*/
				gpio_WritePin(&_IN1, SET);
				gpio_WritePin(&_IN2, RESET);
				gpio_WritePin(&_IN3, SET);
				gpio_WritePin(&_IN4, RESET);
				teclado = 0;
				gpio_ReadPin(&infrarojo_delantero);



			}
			while(teclado == 'd' ) {
			/*derecha*/
				gpio_WritePin(&_IN1, RESET);
				gpio_WritePin(&_IN2, SET);
				gpio_WritePin(&_IN3, RESET);
				gpio_WritePin(&_IN4, RESET);
				teclado = 0;
				gpio_ReadPin(&infrarojo_delantero);



			}
			while (teclado == 'i' ) {
			/*izquierda*/
				gpio_WritePin(&_IN1, RESET);
				gpio_WritePin(&_IN2, RESET);
				gpio_WritePin(&_IN3, RESET);
				gpio_WritePin(&_IN4, SET);
				teclado = 0;
				gpio_ReadPin(&infrarojo_delantero);



			}

			while(teclado == 'u' ) {
			/*izquierda*/
				gpio_WritePin(&_IN1, RESET);
				gpio_WritePin(&_IN2, SET);
				gpio_WritePin(&_IN3, SET);
				gpio_WritePin(&_IN4, RESET);
				teclado = 0;




			}


//				gpio_WritePin(&_IN1, RESET);
//				gpio_WritePin(&_IN2, RESET);
//				gpio_WritePin(&_IN3, RESET);
//				gpio_WritePin(&_IN4, RESET);
//				teclado = 0;


//			else {
//				gpio_WritePin(&_IN1, RESET);
//				gpio_WritePin(&_IN2, RESET);
//				gpio_WritePin(&_IN3, RESET);
//				gpio_WritePin(&_IN4, RESET);
//
//			}



		}
		else {
			gpio_WritePin(&led_Blinky, SET);
			gpio_WritePin(&_IN1, RESET);
			gpio_WritePin(&_IN2, RESET);
			gpio_WritePin(&_IN3, RESET);
			gpio_WritePin(&_IN4, RESET);

		}
	}


//		if (flag_boton ==  0){
			/*Quieto*/
//			if (Lectura_ejeY > 1800 && Lectura_ejeY < 2300 && Lectura_ejeX > 1800 && Lectura_ejeX < 2300) {
//				gpio_WritePin(&_IN1, RESET);
//				gpio_WritePin(&_IN2, RESET);
//				gpio_WritePin(&_IN3, RESET);
//				gpio_WritePin(&_IN4, RESET);
//
//			}
//			/*adelante*/
//			else if (Lectura_ejeY < 300) {
//
//				gpio_WritePin(&_IN1, RESET);
//				gpio_WritePin(&_IN2, SET);
//				gpio_WritePin(&_IN3, RESET);
//				gpio_WritePin(&_IN4, SET);
//
//			}
//			/*atras*/
//			else if (Lectura_ejeY > 3700) {
//
//				gpio_WritePin(&_IN1, SET);
//				gpio_WritePin(&_IN2, RESET);
//				gpio_WritePin(&_IN3, SET);
//				gpio_WritePin(&_IN4, RESET);
//
//			}
//			/*derecha*/
//			else if (Lectura_ejeX < 300) {
//
//				gpio_WritePin(&_IN1, RESET);
//				gpio_WritePin(&_IN2, SET);
//				gpio_WritePin(&_IN3, RESET);
//				gpio_WritePin(&_IN4, RESET);
//
//			}
//			/*izquierda*/
//			else if (Lectura_ejeX > 3700) {
//
//				gpio_WritePin(&_IN1, RESET);
//				gpio_WritePin(&_IN2, RESET);
//				gpio_WritePin(&_IN3, RESET);
//				gpio_WritePin(&_IN4, SET);
//
//			}

//		}
//		else {
//			gpio_WritePin(&_IN1, RESET);
//			gpio_WritePin(&_IN2, SET);
//			gpio_WritePin(&_IN3, SET);
//			gpio_WritePin(&_IN4, RESET);
//			if (teclado == 'g') {
//				usart_writeMsg(&usart, "360 a la Izquierda\n\r");
//				gpio_WritePin(&_IN1, SET);
//				gpio_WritePin(&_IN2, RESET);
//				gpio_WritePin(&_IN3, RESET);
//				gpio_WritePin(&_IN4, SET);
//				teclado = 0;
//			}
//			if (teclado == 'h') {
//				usart_writeMsg(&usart, "360 a la derecha\n\r");
//				gpio_WritePin(&_IN1, RESET);
//				gpio_WritePin(&_IN2, SET);
//				gpio_WritePin(&_IN3, SET);
//				gpio_WritePin(&_IN4, RESET);
//				teclado = 0;
//			}

//		}

//	}
}
/*******************************************************************************************************************************************/
void start(void) {
	/*Configuracion del _SW_cambiaPWM
	 Boton usado para cambiar el sentido del contador */
	_SW_cambiaPWM.pGPIOx = GPIOA;
	_SW_cambiaPWM.pinConfig.GPIO_PinNumber = PIN_1;
	_SW_cambiaPWM.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	_SW_cambiaPWM.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_SW_cambiaPWM.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_SW_cambiaPWM.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	_Exti1Botton.pGPIOHandler = &_SW_cambiaPWM;
	_Exti1Botton.edgeType = EXTERNAL_INTERRUPT_RISING_FALLING_EDGE;
	exti_Config(&_Exti1Botton);
	/*Se configura el led rojo del blinky*/
	led_Blinky.pGPIOx = GPIOB;
	led_Blinky.pinConfig.GPIO_PinNumber = PIN_10;
	led_Blinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	led_Blinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	led_Blinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	led_Blinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&led_Blinky);
//	gpio_WritePin(&led_Blinky, SET);
	/*Se configura el timer para el blinky*/
//	timer_del_Blinky.pTIMx = TIM9;
//	timer_del_Blinky.TIMx_Config.TIMx_Prescaler = 16000;
//	timer_del_Blinky.TIMx_Config.TIMx_Period = 250;
//	timer_del_Blinky.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
//	timer_del_Blinky.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
//	timer_Config(&timer_del_Blinky);
//	/*Que el timer esté encendido*/
//	timer_SetState(&timer_del_Blinky, TIMER_ON);
	/*Se configura el usart de comunicacion serial*/
	usart.ptrUSARTx = USART6;
	usart.USART_Config.baudrate = USART_BAUDRATE_9600;
	usart.USART_Config.datasize = USART_DATASIZE_8BIT;
	usart.USART_Config.parity = USART_PARITY_NONE;
	usart.USART_Config.stopbits = USART_STOPBIT_1;
	usart.USART_Config.mode = USART_MODE_RXTX;
	usart.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart_Config(&usart);
	/*Se configura el pin transmisor*/
	Transmisor.pGPIOx = GPIOA;
	Transmisor.pinConfig.GPIO_PinNumber = PIN_11;
	Transmisor.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Transmisor.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Transmisor.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	Transmisor.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Transmisor.pinConfig.GPIO_PinAltFunMode = AF8;
	gpio_Config(&Transmisor);
	/*Se configura el pin receptor*/
	Receptor.pGPIOx = GPIOA;
	Receptor.pinConfig.GPIO_PinNumber = PIN_12;
	Receptor.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Receptor.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Receptor.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	Receptor.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Receptor.pinConfig.GPIO_PinAltFunMode = AF8;
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
	_PWM_ENB.config.CicloDuty = 1000;
	configuracion_del_pwm(&_PWM_ENB);
	activar_salida(&_PWM_ENB);
	inicio_de_señal_pwm(&_PWM_ENB);

	_IN4.pGPIOx = GPIOB;
	_IN4.pinConfig.GPIO_PinNumber = PIN_2;
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
	gpio_WritePin(&_IN3,SET);


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

	infrarojo_delantero.pGPIOx = GPIOB;
	infrarojo_delantero.pinConfig.GPIO_PinNumber = PIN_5;
	infrarojo_delantero.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	infrarojo_delantero.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	infrarojo_delantero.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	infrarojo_delantero.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&infrarojo_delantero);

	/*El sensor 1 siendo el primero de la secuencia*/
//	ejeX.channel = CHANNEL_10;
//	ejeX.resolution = RESOLUTION_12_BIT;
//	ejeX.dataAlignment = ALIGNMENT_RIGHT;
//	ejeX.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
//	ejeX.interrupState = ADC_INT_ENABLE;
//	/*El sensor 2 siendo el segundo de la secuencia*/
//	ejeY.channel = CHANNEL_8;
//	ejeY.resolution = RESOLUTION_12_BIT;
//	ejeY.dataAlignment = ALIGNMENT_RIGHT;
//	ejeY.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
//	ejeY.interrupState = ADC_INT_ENABLE;
//
//	arreglo_ejes[0] = ejeX;
//	arreglo_ejes[1] = ejeY;
//
//	/*Ahora que se cargue la configuracion de todos los sensores*/
//	adc_ConfigMultichannel (arreglo_ejes,CANTIDAD_DE_SENSORES);
//	/*Se configura el PWM que va a ayudar a muestrear la señal */
//	_PWM_Muestreo.pTIMx = TIM4;
//	_PWM_Muestreo.config.Canal = PWM_CHANNEL_4;
//	_PWM_Muestreo.config.prescaler = 16000;
//	_PWM_Muestreo.config.periodo = 250;
//	_PWM_Muestreo.config.CicloDuty = 20;
//	configuracion_del_pwm(&_PWM_Muestreo);
//	inicio_de_señal_pwm(&_PWM_Muestreo);
//	inicio_de_señal_pwm(&_PWM_Muestreo);
//	/*Se configura el trigger externo con PWM*/
//	adc_ConfigTrigger (TRIGGER_EXT, &_PWM_Muestreo);

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
//void Timer9_Callback(void) {
////	gpio_TooglePin(&led_Blinky);
//}
/*Funcion que si se recibe algo por comunicacion serial almacena la informacion en la variable teclado*/
void usart6_RxCallback(void){
	//leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto además debe bajar la bandera de la interrupción.
	teclado = usart_getRxData();

}

void callback_extInt1(void) {
	flag_boton ^= 1;
}
/*Esta funcion se activa por cada conversion segun la secuencia*/
//void adc_CompleteCallback(void) {
//	arreglo_ejes[_Contador_Secuencia].adcData = adc_GetValue();
//	if (_Contador_Secuencia == 0){
//		Lectura_ejeX = arreglo_ejes[0].adcData;
//	}
//	else if (_Contador_Secuencia == 1){
//		Lectura_ejeY = arreglo_ejes[1].adcData;
//	}
//	_Contador_Secuencia++;
//	/*Se evalua si la secuencia ha llegado a 3, el cual es el numero de canales*/
//	if (_Contador_Secuencia >=CANTIDAD_DE_SENSORES){
//		/*Se reinicia el contador para estar al ritmo de la secuencia de conversion ADC*/
//		_Contador_Secuencia = 0;
//	}
//}
/*FINISH*//*********************************************************************************************************************************/

/*
 *  Main de ADC
 *
 *   Created on: Oct 31, 2023
 *   	 Author:Cristhian Oviedo
 */

#include <stdint.h>
#include "usart_driver_hal.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "adc_driver_hal.h"
#include "pwm_driver_hal.h"


USART_Handler_t usart = { 0 };
GPIO_Handler_t _Tx = { 0 };
GPIO_Handler_t _Rx = { 0 };

Timer_Handler_t blinkTimer = { 0 };
GPIO_Handler_t userLed = { 0 };

char bufferMsg[128] = { 0 };

ADC_Config_t osicloscopio = { 0 };
uint8_t adcComplete = 0;
uint8_t receivedChar = 0;

PWM_Handler_t _PWM_Muestreo = {0};


uint16_t contad = 0;
uint16_t Datos[512];
uint16_t banderafinal = 0;
uint16_t banderainicio = 0;

void initSys(void);

int main() {
	SCB->CPACR |= (0b11 << 20);
	initSys();
	while (1) {
//		if (adcComplete) {
//			adcComplete = 0;
//				sprintf(bufferMsg, " dato ADC %d\n\r",osicloscopio.adcData);
//				usart_writeMsg(&usart, bufferMsg);
//		}
	if (banderafinal){
		sprintf(bufferMsg, " recolecta final");
		usart_writeMsg(&usart, bufferMsg);
		banderafinal =0;
		for (uint16_t l =0; l<512;l++){
			sprintf(bufferMsg, "%u\n",Datos[l]);
			usart_writeMsg(&usart, bufferMsg);

	}
	}
		if (receivedChar != '\0') {
			if (receivedChar == 'q') {
				usart_writeMsg(&usart, "Testing, Testing!!\n\r");
			}
			if (receivedChar == 's') {
				banderainicio = 1;

				usart_writeMsg(&usart, "empieza");
//				adc_StartSingleConv();
			}
			if (receivedChar == 'c') {
				usart_writeMsg(&usart, "make continuous ADC \n\r");
//				adc_StartContinuousConv();
//				pwm_timer_setstate(&_PWM_Muestreo,PWMTIMER_ON);
				inicio_de_señal_pwm(&_PWM_Muestreo);

			}
			if (receivedChar == 'p') {
				usart_writeMsg(&usart, "stop continuous ADC \n\r");
//				adc_StopContinuousConv();
//				pwm_timer_setstate(&_PWM_Muestreo,PWMTIMER_OFF);
				frenar_señal_pwm (&_PWM_Muestreo);
			}
			receivedChar = 0;
		}
	}
	return 0;
}

void initSys(void) {
	/* Configuramos el timer del blink (TIM2) */
	blinkTimer.pTIMx = TIM9;
	blinkTimer.TIMx_Config.TIMx_Prescaler = 16000;
	blinkTimer.TIMx_Config.TIMx_Period = 250;
	blinkTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&blinkTimer);
	timer_SetState(&blinkTimer, SET);

/*Configuramos los pines que se van a utilizar*/

	/* Configuramos el PinA5 */
	userLed.pGPIOx = GPIOB;
	userLed.pinConfig.GPIO_PinNumber = PIN_10;
	userLed.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	gpio_Config(&userLed);

	usart.ptrUSARTx = USART2;
	usart.USART_Config.baudrate = USART_BAUDRATE_115200;
	usart.USART_Config.datasize = USART_DATASIZE_8BIT;
	usart.USART_Config.parity = USART_PARITY_NONE;
	usart.USART_Config.stopbits = USART_STOPBIT_1;
	usart.USART_Config.mode = USART_MODE_RXTX;
	usart.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;

	usart_Config(&usart);

	/* Configuramos el PinA5 */
	_Tx.pGPIOx = GPIOA;
	_Tx.pinConfig.GPIO_PinNumber = PIN_2;
	_Tx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_Tx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Tx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	_Tx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_Tx.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&_Tx);



	/* Configuramos el PinA5 */
	_Rx.pGPIOx = GPIOA;
	_Rx.pinConfig.GPIO_PinNumber = PIN_3;
	_Rx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_Rx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Rx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	_Rx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_Rx.pinConfig.GPIO_PinAltFunMode = AF7;

	gpio_Config(&_Rx);

	osicloscopio.channel = CHANNEL_6;
	osicloscopio.resolution = RESOLUTION_12_BIT;
	osicloscopio.dataAlignment = ALIGNMENT_RIGHT;
	osicloscopio.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;
	osicloscopio.interrupState = ADC_INT_ENABLE;
	adc_ConfigSingleChannel(&osicloscopio);

	_PWM_Muestreo.pTIMx = TIM4;
	_PWM_Muestreo.config.Canal = PWM_CHANNEL_4;
	_PWM_Muestreo.config.prescaler = 16;
	_PWM_Muestreo.config.periodo = 25;
	_PWM_Muestreo.config.CicloDuty = 2;
	configuracion_del_pwm(&_PWM_Muestreo);
//	pwm_timer_setstate(&_PWM_Muestreo,PWMTIMER_ON);
//	activar_salida(&_PWM_Muestreo);
//	inicio_de_señal_pwm(&_PWM_Muestreo);


	/*Se configura el trigger externo*/
	adc_ConfigTrigger (TRIGGER_EXT, &_PWM_Muestreo);

}

void Timer9_Callback(void) {
     gpio_TooglePin(&userLed);

}


void usart2_RxCallback(void) {
	receivedChar = usart_getRxData();

}

void adc_CompleteCallback(void) {
	osicloscopio.adcData= adc_GetValue();
	if(banderainicio){
	Datos[contad] = osicloscopio.adcData;
	contad++;
	    if(contad ==512){
		  contad =0;
		  banderainicio =0;
		  banderafinal =1;
	    }
	}


//	adcComplete = 1;

}

/*
 * Esta funcion sirve para detectar problemas de parametros
 * incorrectos al momento de ejecutar un programa.
 */
void assert_failed(uint8_t *file, uint32_t line) {
	while (1) {
		// problemas...
	}

}

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stm32f4xx.h>
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "exti_driver_hal.h"
#include "timer_driver_hal.h"

#define apagado o
#define encendido 1

// Definicion de las cabeceras de las funciones del main
void initSystem(void);
uint8_t isNumber(char caracter);
uint8_t isLetter(char caracter);
void analyzeCommand(char *buffer);
void configTimerPeriod(Timer_Handler_t *timer, uint16_t period);
void configAdcResolution(ADC_Config_t *adc, uint8_t resolution);

// Definicion de los handlers necesarios
GPIO_Handler_t BlinkyPin = { 0 };
GPIO_Handler_t UserButton = { 0 };
GPIO_Handler_t TxPin = { 0 };
GPIO_Handler_t  RxPin = { 0 };

// Timer encargado del Blinky
Timer_Handler_t BlinkyTimer = { 0 };
Timer_Handler_t samplingTimer = { 0 };

// Comunicación RS-232 con el PC, ya habilitada en la board Nucleo
// Utiliza la conexion USB
USART_Handler_t Usart2 = { 0 };
uint8_t rxData = 0;
char bufferData[64] = { 0 };
char commandBuffer[8] = { 0 };

// Elementos necesarios para configurar el ADC
ADC_Config_t sensor = { 0 };
_bool adcIsComplete = True;
int numberCommand = 0;
uint8_t samplingCounter = 0;

// Mensaje que se imprime
char greetingMsg[] = "Taller V Rocks!\n";

uint8_t commandFlag = 0;
uint8_t display = 1;

uint8_t tipo0 = 0;
/**
 * Funcion principal del programa.
 * Esta funcion es el corazón del programa!!
 *
 * */
int main(void) {

	// Inicializamos todos los elementos del sistema
	initSystem();

	usart_WriteChar(&handlerUsart2, ' ');

	/* Loop forever */
	while (1) {

		if (rxData != 0) {
			if (isLetter(rxData)) {
				usart_WriteChar(&Usart2, rxData);
				strcat(commandBuffer, (char*) &rxData);
			}

			else if (isNumber(rxData)) {
				usart_WriteChar(&Usart2, rxData);
				usart_WriteChar(&Usart2, '\n');
				strcat(commandBuffer, (char*) &rxData);
				commandFlag = 1;
			}
			rxData = 0;
		}

		if (commandFlag) {
			analyzeCommand(commandBuffer);
			for (int i = 0; i < sizeof(commandBuffer); i++) {
				commandBuffer[i] = 0;
			}
			commandFlag = 0;
		}

	}

	return 0;
}

void initSystem(void) {

	// Configurando el pin para el Led_Blinky
	handlerBlinkyPin.pGPIOx = GPIOA;
	handlerBlinkyPin.pinConfig.GPIO_PinNumber = PIN_5;
	handlerBlinkyPin.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinkyPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	handlerBlinkyPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&handlerBlinkyPin);

	// Configurando el pin para el boton azul
	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.pinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserButton.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&handlerUserButton);

	/* Configurando los pines sobre los que funciona el USART2 (TX) */
	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.pinConfig.GPIO_PinNumber = PIN_2;
	handlerTxPin.pinConfig.GPIO_PinMode = GPIO_MODE_ALFTN;
	handlerTxPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerTxPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	handlerTxPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerTxPin.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&handlerTxPin);

	/* Configurando los pines sobre los que funciona el USART2 (RX) */
	handlerRxPin.pGPIOx = GPIOA;
	handlerRxPin.pinConfig.GPIO_PinNumber = PIN_3;
	handlerRxPin.pinConfig.GPIO_PinMode = GPIO_MODE_ALFTN;
	handlerRxPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerRxPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerRxPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	handlerRxPin.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&handlerRxPin);

	// Configurando el Timer2 para que funcione con el blinky
	handlerBlinkyTimer.pTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	handlerBlinkyTimer.TIMx_Config.TIMx_Prescaler = 16000;	// ms
	handlerBlinkyTimer.TIMx_Config.TIMx_Period = 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_ON;
	timer_Config(&handlerBlinkyTimer);
	timer_SetState(&handlerBlinkyTimer, SET);

	// Configurando el Timer5 para que funcione con el blinky
	samplingTimer.pTIMx = TIM5;
	samplingTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	samplingTimer.TIMx_Config.TIMx_Prescaler = 160;	// 10 us
	samplingTimer.TIMx_Config.TIMx_Period = 500;	// ARR = 5000 us
	samplingTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_ON;
	timer_Config(&samplingTimer);
	timer_SetState(&samplingTimer, RESET);			// Inicia apagado

	// Configurando la comunicación serial (Cable verde es TX, Cable Blanco es RX)
	handlerUsart2.ptrUSARTx = USART2;
	handlerUsart2.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity = USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_RX_Int_Ena = SET;
	handlerUsart2.USART_Config.USART_TX_Int_Ena = RESET;
	USART_Config(&handlerUsart2);

	/* Cargando la configuración para la conversión ADC */
	uint8_t channels[1] = { ADC_CHANNEL_1 };
	adcConfig.numberOfChannels = 1;
	adcConfig.channels = channels;
	adcConfig.dataAlignment = ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution = ADC_RESOLUTION_12_BIT;
	uint8_t samplingPeriod[1] = { 0 };
	samplingPeriod[0] = ADC_SAMPLING_PERIOD_56_CYCLES
	;
	adcConfig.samplingPeriod = samplingPeriod;

	/* Configuramos el micro, la interrupción esta activa por defecto.*/
	adc_Config(&adcConfig);
}
void analyzeCommand(char *buffer) {

	if (strcmp(commandBuffer, "h0") == 0) {
		writeString(&handlerUsart2, "Menu de ayuda (help): \n");
		writeString(&handlerUsart2, "1)  h0 -> Se imprime el menu de ayuda \n");
		writeString(&handlerUsart2, "2)  b0 -> Apagar blinky \n");
		writeString(&handlerUsart2, "3)  b1 -> Prender blinky \n");
		writeString(&handlerUsart2,
				"4)  b2 -> Colocar un periodo al blinky de 250 ms  \n");
		writeString(&handlerUsart2,
				"5)  b3 -> Colocar un periodo al blinky de 500 ms  \n");
		writeString(&handlerUsart2,
				"6)  b4 -> Colocar un periodo al blinky de 2 s   \n");
		writeString(&handlerUsart2, "7)  at0 -> Apagar timer del muestreo \n");
		writeString(&handlerUsart2,
				"8)  at1 -> Prender el timer del muestreo  \n");
		writeString(&handlerUsart2,
				"9)  at2 -> Colocar un periodo de muestreo a 5 ms  \n");
		writeString(&handlerUsart2,
				"10) at3 -> Colocar un periodo de muestreo a 50 ms \n");
		writeString(&handlerUsart2,
				"11)  ar0 -> Cambiar la resolución a 12 bits \n");
		writeString(&handlerUsart2,
				"12)  ar1 -> Cambiar la resolución a 10 bits \n");
		writeString(&handlerUsart2,
				"13)  ar2 -> Cambiar la resolución a 8 bits \n");
		writeString(&handlerUsart2,
				"14)  ar3 -> Cambiar la resolución a 6 bits \n");
		writeString(&handlerUsart2,
				"15)  af0 -> Imprimir valor actual en decimal \n");
		writeString(&handlerUsart2,
				"16)  af1 -> Imprimir valor actual en hexadecimal \n");
		writeString(&handlerUsart2,
				"17)  af2 -> Imprimir valor actual en milivoltios \n");
		writeString(&handlerUsart2,
				"18)  ad0 -> Se desabilita el modo continuo \n");
		writeString(&handlerUsart2,
				"19)  ad1 -> Se habilita el modo continuo \n");
	} else if (strcmp(commandBuffer, "b0") == 0) {
		writeString(&handlerUsart2, "Apagando el led de estado \n");
		timer_SetState(&handlerBlinkyTimer, 0);
	} else if (strcmp(commandBuffer, "b1") == 0) {
		writeString(&handlerUsart2, "Prendiendo el led de estado \n");
		timer_SetState(&handlerBlinkyTimer, 1);
	} else if (strcmp(commandBuffer, "b2") == 0) {
		writeString(&handlerUsart2, "Cambiado el periodo del led a 250 ms \n");
		configTimerPeriod(&handlerBlinkyTimer, 250);
	} else if (strcmp(commandBuffer, "b3") == 0) {
		writeString(&handlerUsart2, "Cambiado el periodo del led a 500 ms \n");
		configTimerPeriod(&handlerBlinkyTimer, 500);
	}
	else if (strcmp(commandBuffer, "b4") == 0) {
			writeString(&handlerUsart2, "Cambiado el periodo del led a 2 s \n");
			configTimerPeriod(&handlerBlinkyTimer, 2000);
		}

}
void configTimerPeriod(Timer_Handler_t *timer, uint16_t period) {
	timer_SetState(&handlerBlinkyTimer, 0);
	timer->TIMx_Config.TIMx_Period = period;
	timer_Config(timer);
	timer_SetState(&handlerBlinkyTimer, 1);
}

uint8_t isNumber(char caracter) {
	if (caracter >= '0' && caracter <= '9') {
		return 1;
	}
	return 0;
}

uint8_t isLetter(char caracter) {
	if ((caracter >= 'A' && caracter <= 'Z')
			|| (caracter >= 'a' && caracter <= 'z')) {
		return 1;
	}
	return 0;
}

/*
 * Callback del Timer2 - Hacemos un blinky....
 */
void Timer2_Callback(void) {
	gpio_TooglePin(&handlerBlinkyPin);
}

/* Callback relacionado con la recepción del USART2
 * El puerto es leido en la ISR (para bajar la bandera de la interrupción)
 * El caracter que se lee es devuelto por la función getRxData
 */
void USART2Rx_Callback(void) {
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}

void Timer5_Callback(void) {
	startSingleADC();
}

/* Esta función se ejecuta luego de una conversion ADC
 * (es llamada por la ISR de la conversion ADC)
 */
void adcComplete_Callback(void) {
	adcData = getADC();
	adcIsComplete = true;
}

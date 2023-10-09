/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Cristhian Oviedo
 * @brief          : Solución a Tarea 2
 ******************************************************************************
 */
#include <stdint.h>
#include <stm32f4xx.h>
#include "exti_driver_hal.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"

void initSystem(void);
void escribirEnDisplay(uint8_t number);
void infoDisplay(void);
void banderaInversor(uint8_t number);

// Se define los pines
GPIO_Handler_t _LedBlinky             = {0};    //PIN A5
GPIO_Handler_t _ClockEncoder          = {0};	//PIN C5
GPIO_Handler_t _Data                  = {0};    //PIN A6
GPIO_Handler_t inverterSw             = {0};	//PIN A1
//Definición de los timer, tanto del display (de los transistores)y el timer del blinky
Timer_Handler_t _BlinkyTimer3         = {0};
Timer_Handler_t _DisplayTimer5        = {0};
//eston son la exti del ClK y de boton de encoder
EXTI_Config_t _Exti5Encoder           = {0};
EXTI_Config_t _Exti1Botton            = {0};
//LED indicador de la direccion de modo directo o inverso del conteo
GPIO_Handler_t _LedDirConteo          = {0};    // PIN A4

//Se define los segmentos de los display

GPIO_Handler_t ledA = {0};   		//LED A        	PA10
GPIO_Handler_t ledB = {0};   		//LED B 		PC4
GPIO_Handler_t ledC = {0};   		//LED C 		PA0
GPIO_Handler_t ledD = {0};   		//LED D 		PB5
GPIO_Handler_t ledE = {0};   		//LED E			PB7
GPIO_Handler_t ledF = {0};   		//LED F			PB14
GPIO_Handler_t ledG = {0};   		//LED G			PB10
GPIO_Handler_t _DispOne = {0};      //ANODO 1	   	PC1
GPIO_Handler_t _DispTwo = {0};      //ANODO 2      	PC3

//Definición de variables a utilizar

int8_t contador = 0;
uint8_t flagInverter = 0;
uint8_t flagFlanco = 0;


/*
 * The main function, where everything happens.
 */
int main(void) {
	initSystem();

	//Ciclo del programa
	while (1) {
		//Si la bandera del boton inversor esta abajo:
		if (flagInverter == RESET) {
        //se enciende el led del conteo
						gpio_WritePin(&_LedDirConteo, SET);
       // Si la bandera del boton inversor es arriba
	} else if (flagInverter == SET) {
// se apaga el del de direccion del conteo
						gpio_WritePin(&_LedDirConteo, RESET);
					}
		if (flagFlanco == SET) {

					// Se baja la bandera
			flagFlanco = RESET;
		uint32_t _Lectura = gpio_ReadPin(&_Data);
					if (flagInverter == SET && _Lectura == RESET) {
						// prender el led del para saber en que lado vamos
						contador++;
						//gpio_WritePin(&_LedDirConteo, RESET);
						if (contador > 99) {
							contador = 99;
						}
						if (contador < 0) {
							contador = 0;
						}

					} else if (flagInverter == SET && _Lectura == SET) {

						contador--;

						if (contador > 99) {
							contador = 99;
						}
						if (contador < 0) {
							contador = 0;
						}

					}

					else if (flagInverter == RESET && _Lectura == RESET) {

						contador--;

						if (contador > 99) {
							contador = 99;
						}
						if (contador < 0) {
							contador = 0;
						}
					} else if (flagInverter == RESET && _Lectura == SET) {
						contador++;

						if (contador > 99) {
							contador = 99;
						}
						if (contador < 0) {
							contador = 0;
						}
					}
			infoDisplay();
		}

		}
	}


void initSystem(void) {

	/* Configuramos el pin del blinky */
	_LedBlinky.pGPIOx = GPIOA;
	_LedBlinky.pinConfig.GPIO_PinNumber = PIN_5;
	_LedBlinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_LedBlinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_LedBlinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_LedBlinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_LedBlinky);

	_LedDirConteo.pGPIOx = GPIOA;
	_LedDirConteo.pinConfig.GPIO_PinNumber = PIN_4;
	_LedDirConteo.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_LedDirConteo.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_LedDirConteo.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_LedDirConteo.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_LedDirConteo);



	//Configuracion ledA
	ledA.pGPIOx = GPIOA;
	ledA.pinConfig.GPIO_PinNumber = PIN_10;
	ledA.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledA.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledA.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledA.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledA);

	//Configuracion ledB
	ledB.pGPIOx = GPIOC;
	ledB.pinConfig.GPIO_PinNumber = PIN_4;
	ledB.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledB.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledB.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledB.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledB);

	//Configuracion ledC
	ledC.pGPIOx = GPIOA;
	ledC.pinConfig.GPIO_PinNumber = PIN_0;
	ledC.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledC.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledC.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledC.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledC);

	//Configuracion ledD
	ledD.pGPIOx = GPIOB;
	ledD.pinConfig.GPIO_PinNumber = PIN_5;
	ledD.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledD.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledD.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledD.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledD);

	//Configuracion ledE
	ledE.pGPIOx = GPIOB;
	ledE.pinConfig.GPIO_PinNumber = PIN_7;
	ledE.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledE.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledE.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledE.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledE);

	//Configuracion ledF
	ledF.pGPIOx = GPIOB;
	ledF.pinConfig.GPIO_PinNumber = PIN_14;
	ledF.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledF.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledF.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledF.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledF);

	//Configuracion ledG
	ledG.pGPIOx = GPIOB;
	ledG.pinConfig.GPIO_PinNumber = PIN_10;
	ledG.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledG.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledG.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledG.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledG);

	//Configuración inicial del _DispOne
	_DispOne.pGPIOx = GPIOC;
	_DispOne.pinConfig.GPIO_PinNumber = PIN_1;
	_DispOne.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_DispOne.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_DispOne.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_DispOne.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_DispOne);

	//Configuración inicial del _DispTwo
	_DispTwo.pGPIOx = GPIOC;
	_DispTwo.pinConfig.GPIO_PinNumber = PIN_3;
	_DispTwo.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_DispTwo.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_DispTwo.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_DispTwo.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_DispTwo);

	/*Configuracion del Data del Encoder
	 Cuando DATA == 0 el programa muestra los números de forma ascendente con cada
	 PULL del giro del Encoder y cuando DATA == 1 el programa muestra los números de manera descendente
	 con cada PULL del giro del Encoder */
	_Data.pGPIOx = GPIOA;
	_Data.pinConfig.GPIO_PinNumber = PIN_6;
	_Data.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	_Data.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Data.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_Data.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_Data);

	/* Configuracion clock Encoder, con cada PULL del Clock del Encoder el contador aumenta o disminuya según
	 sea la configuración del Data 0 o 1. */
	_ClockEncoder.pGPIOx = GPIOC;
	_ClockEncoder.pinConfig.GPIO_PinNumber = PIN_5;
	_ClockEncoder.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	_ClockEncoder.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_ClockEncoder.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_ClockEncoder.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	_Exti5Encoder.pGPIOHandler = &_ClockEncoder;
	_Exti5Encoder.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	exti_Config(&_Exti5Encoder);

	/*Configuracion del inverterSw
	 Boton usado para cambiar el sentido del contador */
	inverterSw.pGPIOx = GPIOA;
	inverterSw.pinConfig.GPIO_PinNumber = PIN_1;
	inverterSw.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	inverterSw.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	inverterSw.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	inverterSw.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	_Exti1Botton.pGPIOHandler = &inverterSw;
	_Exti1Botton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	exti_Config(&_Exti1Botton);

	//Configuracion del TIM3
	_BlinkyTimer3.pTIMx = TIM3;
	_BlinkyTimer3.TIMx_Config.TIMx_Prescaler = 16000; // Genera incrementos de 1 ms
	_BlinkyTimer3.TIMx_Config.TIMx_Period = 250; // De la mano con el prescaler
	_BlinkyTimer3.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	_BlinkyTimer3.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	/* Configuramos el Timer3 */
	timer_Config(&_BlinkyTimer3);
	// Encendemos el Timer3.
	timer_SetState(&_BlinkyTimer3, TIMER_ON);

	//Configuracion del TIM5
	_DisplayTimer5.pTIMx = TIM5;
	_DisplayTimer5.TIMx_Config.TIMx_Prescaler = 16000; // Genera incrementos de 1 ms
	_DisplayTimer5.TIMx_Config.TIMx_Period = 10; // De la mano con el prescaler
	_DisplayTimer5.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	_DisplayTimer5.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	/* Configuramos el Timer5 */
	timer_Config(&_DisplayTimer5);
	// Encendemos el Timer5.
	timer_SetState(&_DisplayTimer5, TIMER_ON);

	gpio_WritePin(&_DispOne, SET);
	gpio_WritePin(&_DispTwo, RESET);
	gpio_WritePin(&_LedDirConteo, SET);

	/* Como el 7 segmentos es anodo común, para cambiar estado Led se debe poner en SET y viceversa
	 Se apagan todos los Leds */
	gpio_WritePin(&ledA, RESET);
	gpio_WritePin(&ledB, RESET);
	gpio_WritePin(&ledC, RESET);
	gpio_WritePin(&ledD, RESET);
	gpio_WritePin(&ledE, RESET);
	gpio_WritePin(&ledF, RESET);
	gpio_WritePin(&ledG, RESET);

}

//calllback del timer del Blinkyn
void Timer3_Callback(void) {
	//Se enciende y apaga el LED
	gpio_TooglePin(&_LedBlinky);
}

//calllback timer de los transistores que controlan el 7 segmento
void Timer5_Callback(void) {

	//Cada vez que interrumpe TIM5 se cambia el estado de los dos pines para hacer el switch
	gpio_TooglePin(&_DispOne);
	gpio_TooglePin(&_DispTwo);
	infoDisplay();

}


//callback del boton inversor
void callback_extInt1(void) {
// Bandera que cada vez que se presiona el SW del encoder flagInverter cambia su valor alternando de 1 a 0 por el XOR
	flagInverter ^= 1;

}


//calllback del CLock
void callback_extInt5(void) {
	//se sube una bandera si hay una interrupcion girando el encoder
	flagFlanco = SET;

	/*
	 * Función que indica qué debe mostrar cada uno de los displays de las unidades y las decenas
	 */
}

void infoDisplay(void) {
	uint32_t left = gpio_ReadPin(&_DispOne);
	if (left == SET) {
		escribirEnDisplay(contador / 10);
	}
	uint32_t right = gpio_ReadPin(&_DispTwo);
	if (right == SET) {
		escribirEnDisplay(contador % 10);
	}
}
//Funcion que define los numeros dependiendo de los casos
void escribirEnDisplay(uint8_t number) {

	switch (number) {
	case 0: {
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, RESET);
		gpio_WritePin(&ledF, RESET);
		gpio_WritePin(&ledG, SET);
		break;
	}
	case 1: {
		gpio_WritePin(&ledA, SET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, SET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledF, SET);
		gpio_WritePin(&ledG, SET);
		break;
	}
	case 2: {
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, SET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, RESET);
		gpio_WritePin(&ledF, SET);
		gpio_WritePin(&ledG, RESET);
		break;
	}

	case 3: {
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledF, SET);
		gpio_WritePin(&ledG, RESET);
		break;
	}
	case 4: {
		gpio_WritePin(&ledA, SET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, SET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledF, RESET);
		gpio_WritePin(&ledG, RESET);

		break;
	}
	case 5: {
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, SET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledF, RESET);
		gpio_WritePin(&ledG, RESET);
		break;
	}
	case 6: {
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, SET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, RESET);
		gpio_WritePin(&ledF, RESET);
		gpio_WritePin(&ledG, RESET);
		break;
	}

	case 7: {
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, SET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledF, SET);
		gpio_WritePin(&ledG, SET);
		break;
	}

	case 8: {
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, RESET);
		gpio_WritePin(&ledF, RESET);
		gpio_WritePin(&ledG, RESET);
		break;
	}
	case 9: {
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledF, RESET);
		gpio_WritePin(&ledG, RESET);
		break;
	}
	default: {
		gpio_WritePin(&ledA, SET);
		gpio_WritePin(&ledB, SET);
		gpio_WritePin(&ledC, SET);
		gpio_WritePin(&ledD, SET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledF, SET);
		gpio_WritePin(&ledG, SET);
		break;

	}

	}

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

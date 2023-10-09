/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Cristhian Oviedo
 * @brief          : Basic project, base for all new projects
 ******************************************************************************
 */
#include <stdint.h>
#include <stm32f4xx.h>
#include "exti_driver_hal.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"

void initSystem(void);
void displayWrite(uint8_t number);
void switchDigit(void);
void wormPath(uint8_t number);

// Definimos los pines
GPIO_Handler_t handlerLedBlinky       = {0};     //PIN A5
GPIO_Handler_t handlerClockEncoder    = {0};	//PIN A6
GPIO_Handler_t handlerData            = {0};     //PIN A7
GPIO_Handler_t handlerbotoninversor   = {0};	//PIN A1
//los timer de display (de los transistores)y el timer del blinky
Timer_Handler_t handlerBlinkyTimer3   = {0};
Timer_Handler_t handlerDisplayTimer5  = {0};
//eston son la exti del ClK y de boton de encoder
EXTI_Config_t exti6Encoder            = {0};
EXTI_Config_t exti1Button             = {0};
//led que indica el tipo de modo directo o inverso
GPIO_Handler_t LedTipoModo            = {0};    // PIN B4

//Declaracion de los pines del display

GPIO_Handler_t ledA = {0};   		//LED A        	PA3
GPIO_Handler_t ledB = {0};   		//LED B 		PA4
GPIO_Handler_t ledC = {0};   		//LED C 		PA11
GPIO_Handler_t ledD = {0};   		//LED D 		PA2
GPIO_Handler_t ledE = {0};   		//LED E			PA10
GPIO_Handler_t ledF = {0};   		//LED F			PA9
GPIO_Handler_t ledG = {0};   		//LED G			PA8
GPIO_Handler_t DispUni = {0};   	//ANODO 1	   	PA12
GPIO_Handler_t DispDec = {0};  		//ANODO 2      	PA0

//variables a
int8_t contador = 0;
uint8_t modeWorm = 0;
int8_t banderadeinversor = 0;
uint8_t buttonFlag = 0;




/*
 * The main function, where everything happens.
 */
int main(void) {
	initSystem();

	//Ciclo del programa
	while (1) {
		if (modeWorm == RESET) {

						gpio_WritePin(&LedTipoModo, SET);
	} else if (modeWorm == SET) {

						gpio_WritePin(&LedTipoModo, RESET);
					}
		if (buttonFlag == SET) {

					// Se baja la bandera
			buttonFlag = RESET;
		uint32_t button = gpio_ReadPin(&handlerData);
					if (modeWorm == SET && button == RESET) {
						// prender el led del para saber en que lado vamos
						contador++;
						//gpio_WritePin(&LedTipoModo, RESET);
						if (contador > 99) {
							contador = 99;
						}
						if (contador < 0) {
							contador = 0;
						}

					} else if (modeWorm == SET && button == SET) {

						contador--;
//gpio_WritePin(&LedTipoModo, RESET);
						if (contador > 99) {
							contador = 99;
						}
						if (contador < 0) {
							contador = 0;
						}

					}

					else if (modeWorm == RESET && button == RESET) {

						contador--;
						//gpio_WritePin(&LedTipoModo, SET);

						if (contador > 99) {
							contador = 99;
						}
						if (contador < 0) {
							contador = 0;
						}
					} else if (modeWorm == RESET && button == SET) {
						contador++;
						//gpio_WritePin(&LedTipoModo, SET);

						if (contador > 99) {
							contador = 99;
						}
						if (contador < 0) {
							contador = 0;
						}
					}


		/*uint32_t button = gpio_ReadPin(&handlerData);
		if (banderadeinversor == RESET){

		}
		if (buttonFlag == SET)  {

			// Se baja la bandera
			buttonFlag = RESET;



			gpio_WritePin(&LedTipoModo, SET);




			if (button == SET) {
				// prender el led del para saber en que lado vamos
				contador++;
				if (contador > 99) {
					contador = 99;
				}

			}
			if (button == RESET){

				contador--;

				if (contador < 0) {
					contador = 0;
				}

			}

		 if (buttonFlag == SET ) {

			  //Se baja la bandera
			 buttonFlag = RESET;

			gpio_WritePin(&LedTipoModo, RESET);


			if (button == SET) {

				contador--;

				if (contador < 0) {
					contador = 0;
				}
			}
			if (button == RESET) {
				contador++;

				if (contador > 99) {
					contador = 99;
				}
			}
		  }*/
			switchDigit();
		}

		}
	}





void initSystem(void) {

	/* Configuramos el pin del blinky */
	handlerLedBlinky.pGPIOx = GPIOA;
	handlerLedBlinky.pinConfig.GPIO_PinNumber = PIN_5;
	handlerLedBlinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLedBlinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerLedBlinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	handlerLedBlinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&handlerLedBlinky);

	LedTipoModo.pGPIOx = GPIOB;
	LedTipoModo.pinConfig.GPIO_PinNumber = PIN_4;
	LedTipoModo.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	LedTipoModo.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	LedTipoModo.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	LedTipoModo.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&LedTipoModo);



	//Configuracion ledA
	ledA.pGPIOx = GPIOC;
	ledA.pinConfig.GPIO_PinNumber = PIN_3;
	ledA.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledA.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledA.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledA.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledA);

	//Configuracion ledB
	ledB.pGPIOx = GPIOA;
	ledB.pinConfig.GPIO_PinNumber = PIN_4;
	ledB.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledB.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledB.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledB.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledB);

	//Configuracion ledC
	ledC.pGPIOx = GPIOA;
	ledC.pinConfig.GPIO_PinNumber = PIN_11;
	ledC.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledC.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledC.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledC.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledC);

	//Configuracion ledD
	ledD.pGPIOx = GPIOC;
	ledD.pinConfig.GPIO_PinNumber = PIN_6;
	ledD.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledD.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledD.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledD.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledD);

	//Configuracion ledE
	ledE.pGPIOx = GPIOA;
	ledE.pinConfig.GPIO_PinNumber = PIN_10;
	ledE.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledE.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledE.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledE.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledE);

	//Configuracion ledF
	ledF.pGPIOx = GPIOA;
	ledF.pinConfig.GPIO_PinNumber = PIN_9;
	ledF.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledF.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledF.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledF.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledF);

	//Configuracion ledG
	ledG.pGPIOx = GPIOA;
	ledG.pinConfig.GPIO_PinNumber = PIN_8;
	ledG.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledG.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledG.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledG.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledG);

	//Configuración inicial del DispUni
	DispUni.pGPIOx = GPIOA;
	DispUni.pinConfig.GPIO_PinNumber = PIN_12;
	DispUni.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	DispUni.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	DispUni.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	DispUni.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&DispUni);

	//Configuración inicial del DispDec
	DispDec.pGPIOx = GPIOA;
	DispDec.pinConfig.GPIO_PinNumber = PIN_0;
	DispDec.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	DispDec.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	DispDec.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	DispDec.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&DispDec);

	/*Configuracion del Data del Encoder
	 Cuando DATA == 0 el programa muestra los números de forma ascendente con cada
	 PULL del giro del Encoder y cuando DATA == 1 el programa muestra los números de manera descendente
	 con cada PULL del giro del Encoder */
	handlerData.pGPIOx = GPIOA;
	handlerData.pinConfig.GPIO_PinNumber = PIN_7;
	handlerData.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerData.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerData.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	handlerData.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&handlerData);

	/* Configuracion clock Encoder, con cada PULL del Clock del Encoder el contador aumenta o disminuya según
	 sea la configuración del Data que se presenta más adelante. */
	handlerClockEncoder.pGPIOx = GPIOA;
	handlerClockEncoder.pinConfig.GPIO_PinNumber = PIN_6;
	handlerClockEncoder.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerClockEncoder.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerClockEncoder.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	handlerClockEncoder.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	exti6Encoder.pGPIOHandler = &handlerClockEncoder;
	exti6Encoder.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	exti_Config(&exti6Encoder);

	/*Configuracion del handlerbotoninversor
	 Boton usado para cambiar el sentido del contador */
	handlerbotoninversor.pGPIOx = GPIOA;
	handlerbotoninversor.pinConfig.GPIO_PinNumber = PIN_1;
	handlerbotoninversor.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerbotoninversor.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	handlerbotoninversor.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	handlerbotoninversor.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	exti1Button.pGPIOHandler = &handlerbotoninversor;
	exti1Button.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	exti_Config(&exti1Button);

	//Configuracion del TIM3
	handlerBlinkyTimer3.pTIMx = TIM3;
	handlerBlinkyTimer3.TIMx_Config.TIMx_Prescaler = 16000; // Genera incrementos de 1 ms
	handlerBlinkyTimer3.TIMx_Config.TIMx_Period = 250; // De la mano con el prescaler
	handlerBlinkyTimer3.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	handlerBlinkyTimer3.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	/* Configuramos el Timer3 */
	timer_Config(&handlerBlinkyTimer3);
	// Encendemos el Timer3.
	timer_SetState(&handlerBlinkyTimer3, TIMER_ON);

	//Configuracion del TIM5
	handlerDisplayTimer5.pTIMx = TIM5;
	handlerDisplayTimer5.TIMx_Config.TIMx_Prescaler = 16000; // Genera incrementos de 1 ms
	handlerDisplayTimer5.TIMx_Config.TIMx_Period = 10; // De la mano con el prescaler
	handlerDisplayTimer5.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	handlerDisplayTimer5.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	/* Configuramos el Timer5 */
	timer_Config(&handlerDisplayTimer5);
	// Encendemos el Timer5.
	timer_SetState(&handlerDisplayTimer5, TIMER_ON);

	gpio_WritePin(&DispUni, SET);
	gpio_WritePin(&DispDec, RESET);
	gpio_WritePin(&LedTipoModo, SET);

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
	gpio_TooglePin(&handlerLedBlinky);
}

//calllback timer de los transistores que controlan el 7 segmento
void Timer5_Callback(void) {

	//Cada vez que interrumpe TIM5 se cambia el estado de los dos pines para hacer el switch
	gpio_TooglePin(&DispUni);
	gpio_TooglePin(&DispDec);
	switchDigit();

}


/*
 * Si el modo inversor está en RESET entonces el programa ejecuta la parte del
 * código que corresponda a la función de subir y bajar el contador del 0 al 99
 * de forma ascendente o descendente
 */
void callback_extInt1(void) {

	modeWorm ^= 1;

}
/*
 * Interrupción que activa el modo inversion del contador
 */

//calllback del CLK
void callback_extInt6(void) {
	buttonFlag = SET;

	/*
	 * Función que indica qué debe mostrar cada uno de los displays de las unidades y las decenas
	 */

}

void switchDigit(void) {
	uint32_t left = gpio_ReadPin(&DispUni);
	if (left == SET) {
		displayWrite(contador / 10);
	}
	uint32_t right = gpio_ReadPin(&DispDec);
	if (right == SET) {
		displayWrite(contador % 10);
	}
}

void displayWrite(uint8_t number) {

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
 * Función que determina cuales Leds deben estar encendidos dependiendo de la posición
 * en la que se encuentre el gusanito en este momento.
 */

/*
 * Esta funcion sirve para detectar problemas de parametros
 * incorrectos al momento de ejecutar un programa.
 */
void assert_failed(uint8_t *file, uint32_t line) {
	while (1) {
		// problemas...
	}

}

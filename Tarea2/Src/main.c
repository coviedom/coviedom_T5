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

void displayWrite(uint8_t number);
void switchDigit(void);
void wormPath(uint8_t number);

// Definimos los Pin, los Exti y los Timer que vamos a utilizar
GPIO_Handler_t handlerLedBlinky      = {0};     //Pin A5
GPIO_Handler_t handlerClockEncoder 	 = {0};		//PIN H1
GPIO_Handler_t handlerData			 = {0};     //PIN C9
GPIO_Handler_t handlerbotoninversor	 = {0};		//PIN C5
Timer_Handler_t handlerBlinkyTimer3  = {0};
Timer_Handler_t handlerDisplayTimer5 = {0};
EXTI_Config_t exti1Encoder           = {0};
EXTI_Config_t exti5Button            = {0};

int8_t contador 	= 0;
uint8_t modeWorm 	= 0;
int8_t wormCounter 	= 0;


//Declaracion de los pines del display

GPIO_Handler_t 	ledA 			= {0};   		//LED A        	PA1
GPIO_Handler_t 	ledB 	 		= {0};   		//LED B 		PA4
GPIO_Handler_t 	ledC 	 		= {0};   		//LED C 		PB13
GPIO_Handler_t 	ledD 			= {0};   		//LED D 		PB1
GPIO_Handler_t 	ledE 			= {0};   		//LED E			PB15
GPIO_Handler_t 	ledF 			= {0};   		//LED F			PB0
GPIO_Handler_t 	ledG 	 		= {0};   		//LED G			PB2
GPIO_Handler_t 	DispUni 		= {0};   		//ANODO 1	   	PC1
GPIO_Handler_t 	DispDec 		= {0};  		//ANODO 2      	PA0


/*
 * The main function, where everything happens.
 */
int main(void)
{
	 /* Configuramos el pin del blinky */
	handlerLedBlinky.pGPIOx								=GPIOA;
	handlerLedBlinky.pinConfig.GPIO_PinNumber			=PIN_5;
	handlerLedBlinky.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	handlerLedBlinky.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	handlerLedBlinky.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	handlerLedBlinky.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLedBlinky);

	//Configuracion del TIM3
	handlerBlinkyTimer3.pTIMx								= TIM3;
	handlerBlinkyTimer3.TIMx_Config.TIMx_Prescaler			= 16000; // Genera incrementos de 1 ms
	handlerBlinkyTimer3.TIMx_Config.TIMx_Period				= 250;   // De la mano con el prescaler
	handlerBlinkyTimer3.TIMx_Config.TIMx_mode				= TIMER_UP_COUNTER;
	handlerBlinkyTimer3.TIMx_Config.TIMx_InterruptEnable	= TIMER_INT_ENABLE;
	/* Configuramos el Timer3 */
	timer_Config(&handlerBlinkyTimer3);
	// Encendemos el Timer3.
	timer_SetState(&handlerBlinkyTimer3, TIMER_ON);

	//Configuracion del TIM5
	handlerDisplayTimer5.pTIMx								= TIM5;
	handlerDisplayTimer5.TIMx_Config.TIMx_Prescaler		    = 16000; // Genera incrementos de 1 ms
	handlerDisplayTimer5.TIMx_Config.TIMx_Period			= 100;   // De la mano con el prescaler
	handlerDisplayTimer5.TIMx_Config.TIMx_mode				= TIMER_UP_COUNTER;
	handlerDisplayTimer5.TIMx_Config.TIMx_InterruptEnable	= TIMER_INT_ENABLE;
	/* Configuramos el Timer5 */
	timer_Config(&handlerDisplayTimer5);
	// Encendemos el Timer5.
	timer_SetState(&handlerDisplayTimer5, TIMER_ON);

	//Configuracion ledA
	ledA.pGPIOx								=GPIOA;
	ledA.pinConfig.GPIO_PinNumber			=PIN_1;
	ledA.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	ledA.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	ledA.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	ledA.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&ledA);

	//Configuracion ledB
	ledB.pGPIOx								=GPIOA;
	ledB.pinConfig.GPIO_PinNumber			=PIN_4;
	ledB.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	ledB.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	ledB.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	ledB.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&ledB);

	//Configuracion ledC
	ledC.pGPIOx								=GPIOB;
	ledC.pinConfig.GPIO_PinNumber			=PIN_13;
	ledC.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	ledC.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	ledC.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	ledC.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&ledC);

	//Configuracion ledD
	ledD.pGPIOx								=GPIOB;
	ledD.pinConfig.GPIO_PinNumber			=PIN_1;
	ledD.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	ledD.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	ledD.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	ledD.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&ledD);

	//Configuracion ledE
	ledE.pGPIOx								=GPIOB;
	ledE.pinConfig.GPIO_PinNumber			=PIN_15;
	ledE.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	ledE.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	ledE.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	ledE.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&ledE);

	//Configuracion ledF
	ledF.pGPIOx								=GPIOB;
	ledF.pinConfig.GPIO_PinNumber			=PIN_0;
	ledF.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	ledF.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	ledF.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	ledF.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&ledF);

	//Configuracion ledG
	ledG.pGPIOx								=GPIOB;
	ledG.pinConfig.GPIO_PinNumber			=PIN_2;
	ledG.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	ledG.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	ledG.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	ledG.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&ledG);

	//Configuración inicial del DispUni
	DispUni.pGPIOx								=GPIOC;
	DispUni.pinConfig.GPIO_PinNumber			=PIN_1;
	DispUni.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	DispUni.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	DispUni.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	DispUni.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&DispUni);

	//Configuración inicial del DispDec
	DispDec.pGPIOx								=GPIOA;
	DispDec.pinConfig.GPIO_PinNumber			=PIN_0;
	DispDec.pinConfig.GPIO_PinMode				=GPIO_MODE_OUT;
	DispDec.pinConfig.GPIO_PinOutputType		=GPIO_OTYPE_PUSHPULL;
	DispDec.pinConfig.GPIO_PinOutputSpeed		=GPIO_OSPEED_FAST;
	DispDec.pinConfig.GPIO_PinPuPdControl		=GPIO_PUPDR_NOTHING;
	GPIO_Config(&DispDec);

	/* Configuracion clock Encoder, con cada PULL del Clock del Encoder el contador aumenta o disminuya según
       sea la configuración del Data que se presenta más adelante. */
	handlerClockEncoder.pGPIOx						  =GPIOH;
	handlerClockEncoder.pinConfig.GPIO_PinNumber	  =PIN_1;
	handlerClockEncoder.pinConfig.GPIO_PinMode		  =GPIO_MODE_IN;
	handlerClockEncoder.pinConfig.GPIO_PinOutputType  =GPIO_OTYPE_PUSHPULL;
	handlerClockEncoder.pinConfig.GPIO_PinOutputSpeed =GPIO_OSPEED_FAST;
	handlerClockEncoder.pinConfig.GPIO_PinPuPdControl =GPIO_PUPDR_NOTHING;

	exti1Encoder.pGPIOHandler = &handlerClockEncoder;
	exti1Encoder.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	exti_Config(&exti1Encoder);

	/*Configuracion del Data del Encoder
	 Cuando DATA == 0 el programa muestra los números de forma ascendente con cada
	 PULL del giro del Encoder y cuando DATA == 1 el programa muestra los números de manera descendente
	 con cada PULL del giro del Encoder */
	handlerData.pGPIOx						  =GPIOC;
	handlerData.pinConfig.GPIO_PinNumber	  =PIN_9;
	handlerData.pinConfig.GPIO_PinMode		  =GPIO_MODE_IN;
	handlerData.pinConfig.GPIO_PinOutputType  =GPIO_OTYPE_PUSHPULL;
	handlerData.pinConfig.GPIO_PinOutputSpeed =GPIO_OSPEED_FAST;
	handlerData.pinConfig.GPIO_PinPuPdControl =GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerData);

	/*Configuracion del handlerbotoninversor
	Boton usado para cambiar el sentido del contador */
	handlerbotoninversor.pGPIOx						   =GPIOC;
	handlerbotoninversor.pinConfig.GPIO_PinNumber	   =PIN_5;
	handlerbotoninversor.pinConfig.GPIO_PinMode		   =GPIO_MODE_IN;
	handlerbotoninversor.pinConfig.GPIO_PinOutputType  =GPIO_OTYPE_PUSHPULL;
	handlerbotoninversor.pinConfig.GPIO_PinOutputSpeed =GPIO_OSPEED_FAST;
	handlerbotoninversor.pinConfig.GPIO_PinPuPdControl =GPIO_PUPDR_NOTHING;

	exti5Buttonn.pGPIOHandler= &handlerbotoninversor;
	exti5Button.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	exti_Config(&exti5Button);

	gpio_WritePin(&DispUni,SET);
	gpio_WritePin(&DispDec,RESET);

	/* Como el 7 segmentos es anodo común, para apagar un Led se debe poner en SET y viceversa
    Se apagan todos los Leds */
	gpio_WritePin(&ledA, SET);
	gpio_WritePin(&ledB, SET);
	gpio_WritePin(&ledC, SET);
	gpio_WritePin(&ledD, SET);
	gpio_WritePin(&ledE, SET);
	gpio_WritePin(&ledF, SET);
	gpio_WritePin(&ledG, SET);

	contador = 0;


	//Ciclo del programa
    while(1){

	}
    return 0;
}

/*
 *El Timer2_Callback se encanga de que el blinky permanezca encendido mientras el programa
 *se esté ejecutando
 */
void Timer3_Callback(void){
	gpio_TooglePin(&handlerLedBlinky);
}

/*
 * EL BasicTimer4_Callback es el que está "pendiente" de que cuando se genere la interrupción
 * que corresponde al AuxButton
 */
void Timer5_Callback(void){
	if(modeWorm == RESET){

		//Cada vez que interrumpe TIM4 se cambia el estado de los dos pines para hacer el switch
		gpio_TooglePin(&DispUni);
		gpio_TooglePin(&DispDec);
		switchDigit();
	}else{
		wormPath(wormCounter);
	}
}



/*
 * Esta funcion sirve para detectar problemas de parametros
 * incorrectos al momento de ejecutar un programa.
 */
void assert_failed(uint8_t* file, uint32_t line){
	while(1){
		// problemas...
	}
}



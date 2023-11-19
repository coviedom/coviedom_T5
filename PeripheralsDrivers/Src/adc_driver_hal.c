/*
 * adc_driver_hal.c
 *
 *  Created on: Oct 25, 2023
 *      Author: Cristhian Oviedo
 */
#include "adc_driver_hal.h"
#include "gpio_driver_hal.h"
#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32_assert.h>
#include <pwm_driver_hal.h>

/* === Headers for private funcions === */
static void adc_enable_clock_peripheral(void);
static void adc_set_resolution(ADC_Config_t *adcConfig);
static void adc_set_alignment(ADC_Config_t *adcConfig);
static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig);
static void adc_set_one_channel_sequence(ADC_Config_t *adcConfig);
static void adc_config_interrupt(ADC_Config_t *adcConfig);

void adc_ConfigMultichannel (ADC_Config_t *adcConfig, uint8_t numeroDeCanales);
void adc_ConfigTrigger (uint8_t sourceType, Pwm_Handler_t*triggerSignal);

/* Variables y elementos que necesita internamente el driver para funcionar adecuadamente */
GPIO_Handler_t handlerADCPin = { 0 };
uint16_t adcRawData = 0;

void adc_ConfigSingleChannel(ADC_Config_t *adcConfig) {
	/* 1. Configuramos el pinX para que cumpla la funcion del canal analogo deseado */
	adc_ConfigAnalogPin(adcConfig->channel);

	/* 2. Activamos la senal de reloj para el ADC*/
	adc_enable_clock_peripheral();

	/*limpiamos los registros antes de comenzar a configurar*/
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;
	ADC1->SQR1 = 0;

	/*Comenzamos la configuracion de ADC1*/

	/* 3. Resolucion del ADC */
	adc_set_resolution(adcConfig);

	/* 4. Configuramos el modo Scan como desactivado*/
	adc_ScanMode(SCAN_OFF);

	/* 5. Configuramos la alineacion de datos (derecha o izquierda)*/
	adc_set_alignment(adcConfig);

	/* 6. Desactivamos el "continuous mode" */
	adc_StopContinuousConv();

	/* 7. Aca se deberia configurar el sampling*/
	adc_set_sampling_and_hold(adcConfig);

	/* 8. Configuramos la secuencia y cuantos elementos hay en la secuencias */
	adc_set_one_channel_sequence(adcConfig);

	/* 9. Configuramos el prescaler del ADC en 2:1(el más rápido que se puede tener)*/
	ADC->CCR &= ~ADC_CCR_ADCPRE;


	/* 10. Desactivamos las interrupciones globales */
	__disable_irq();

	/* 11. configuramos la interrupción (si se encuentra activa), ademas de inscribir/ remover
	 la interrupcion en el NVIC*/
	adc_config_interrupt(adcConfig);

	/* 12. Activamos los modulos ADC */
	adc_peripheralOnOFF(ADC_ON);

	/* 13. Activamos las interrupciones globales */
	__enable_irq();

}

/*
 * Se activa la señal de reloj para el periferico ADC 1
 */
static void adc_enable_clock_peripheral(void) {
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}
/*
 * Se configura la resolución para la conversión
 */
static void adc_set_resolution(ADC_Config_t *adcConfig) {

	switch (adcConfig->resolution) {

	case RESOLUTION_12_BIT: {
		/*Cargar 0b00*/
		ADC1->CR1 &= ~ADC_CR1_RES;
		break;
	}
	case RESOLUTION_10_BIT: {
		/*Cargar 0b01*/
		ADC1->CR1 |= ADC_CR1_RES_0;
		break;
	}
	case RESOLUTION_8_BIT: {
		/*Cargar 0b10*/
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}
	case RESOLUTION_6_BIT: {
		/*Cargar 0b11*/
		ADC1->CR1 |= ADC_CR1_RES;
		break;
	}
	default: {
		/*Que sea 12 bit*/
		ADC1->CR1 &= ~ADC_CR1_RES;
		break;
	}
	}

}

/*
 * Alineación de derecha o izquierda
 */
static void adc_set_alignment(ADC_Config_t *adcConfig) {

	if (adcConfig->dataAlignment == ALIGNMENT_LEFT) {
		/*Que se cargue alineacion a la izquierda*/
		ADC1->CR2 |= ADC_CR2_ALIGN;
	} else {
		/*Sino alineacion a la derecha*/
		ADC1->CR2 &= ~ADC_CR2_ALIGN;
	}
}
/*
 * Tiene que ver con tiempo de carga del capacitor
 */
static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig) {
	/*Se hace para los 16 canales*/
	if (adcConfig->channel<CHANNEL_10){
		ADC1 -> SMPR2 |= (adcConfig -> samplingPeriod <<(3*(adcConfig->channel)));
	}
	else{
		ADC1 -> SMPR1 |= (adcConfig -> samplingPeriod<<(3*((adcConfig->channel)-10)));
	}
}

/*
 *  Configuramos el número de elementos en secuencia (solo un elemento)
 */
static void adc_set_one_channel_sequence(ADC_Config_t *adcConfig) {
	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	/* Se configura cual es el canal que adquiere la señal ADC*/

	switch (adcConfig->channel) {

	case CHANNEL_0:
		ADC1->SQR3 &= ~ADC_SQR3_SQ1;
		break;

	case CHANNEL_1:
		ADC1->SQR3 |= ADC_SQR3_SQ1_0;
		break;

	case CHANNEL_2:
		ADC1->SQR3 |= ADC_SQR3_SQ1_1;
		break;

	case CHANNEL_3:
		ADC1->SQR3 |= ADC_SQR3_SQ1_0;
		ADC1->SQR3 |= ADC_SQR3_SQ1_1;
		break;

	case CHANNEL_4:
		ADC1->SQR3 |= ADC_SQR3_SQ1_2;
		break;

	case CHANNEL_5:
		ADC1->SQR3 |= ADC_SQR3_SQ1_0;
		ADC1->SQR3 |= ADC_SQR3_SQ1_2;
		break;

	case CHANNEL_6:
		ADC1->SQR3 |= ADC_SQR3_SQ1_1;
		ADC1->SQR3 |= ADC_SQR3_SQ1_2;
		break;

	case CHANNEL_7:
		ADC1->SQR3 |= ADC_SQR3_SQ1_0;
		ADC1->SQR3 |= ADC_SQR3_SQ1_1;
		ADC1->SQR3 |= ADC_SQR3_SQ1_2;
		break;

	case CHANNEL_8:
		ADC1->SQR3 |= ADC_SQR3_SQ1_3;
		break;

	case CHANNEL_9:
		ADC1->SQR3 |= ADC_SQR3_SQ1_0;
		ADC1->SQR3 |= ADC_SQR3_SQ1_3;
		break;

	case CHANNEL_10:
		ADC1->SQR3 |= ADC_SQR3_SQ1_3 | ADC_SQR3_SQ1_1;
		break;

	case CHANNEL_11:
		ADC1->SQR3 |= ADC_SQR3_SQ1_0;
		ADC1->SQR3 |= ADC_SQR3_SQ1_4;
		break;

	case CHANNEL_12:
		ADC1->SQR3 |= ADC_SQR3_SQ1_2;
		ADC1->SQR3 |= ADC_SQR3_SQ1_3;
		break;

	case CHANNEL_13:
		ADC1->SQR3 |= ADC_SQR3_SQ1_0;
		ADC1->SQR3 |= ADC_SQR3_SQ1_2;
		ADC1->SQR3 |= ADC_SQR3_SQ1_3;
		break;

	case CHANNEL_14:
		ADC1->SQR3 |= ADC_SQR3_SQ1_3 | ADC_SQR3_SQ1_2 | ADC_SQR3_SQ1_1;
		break;

	case CHANNEL_15:
		ADC1->SQR3 |= ADC_SQR3_SQ1_0;
		ADC1->SQR3 |= ADC_SQR3_SQ1_2;
		ADC1->SQR3 |= ADC_SQR3_SQ1_4;
		break;

	}

}
/*Configurar la activación de la interrupción y la activación del NVIC*/
static void adc_config_interrupt(ADC_Config_t *adcConfig) {
	if (adcConfig->interrupState == ADC_INT_ENABLE) {
		ADC1->CR1 |= ADC_CR1_EOCIE;
		/*Matriculando la interrupción*/
		__NVIC_EnableIRQ(ADC_IRQn);
		__NVIC_SetPriority(ADC_IRQn, 4);
	} else if (adcConfig->interrupState == ADC_INT_DISABLE) {
		ADC1->CR1 &= ~ADC_CR1_EOCIE;
	} else {
		__NOP();
	}

}
/*
 * Controla la activacion y desactivacion del modulo ADC desde el registro
 * CR2 del ADC
 */
void adc_peripheralOnOFF(uint8_t state) {
	if (state == ADC_ON) {
		ADC1->CR2 |= ADC_CR2_ADON;
	} else if (state == ADC_OFF) {
		ADC1->CR2 &= ~ADC_CR2_ADON;
	}

}
/*
 * Enables and disables the Scan mode.....
 * Funciön de la mano con la secuencia de varios canales
 * No es necesario para el caso de un solo canal simple.
 */
void adc_ScanMode(uint8_t state) {
	if (state == SCAN_OFF) {
		ADC1->CR1 &= ~ADC_CR1_SCAN;
	} else if (state == SCAN_ON) {
		ADC1->CR1 |= ADC_CR1_SCAN;
	}
}
/*Funcion que comienza la conversion ADC simple*/
void adc_StartSingleConv(void) {
	ADC1->CR2 &= ~ADC_CR2_CONT;
	ADC1->CR2 |= ADC_CR2_SWSTART;
}
/*Funcion que comienza la conversion ADC continua*/
void adc_StartContinuousConv(void) {

	ADC1->CR2 |= ADC_CR2_SWSTART;
	ADC1->CR2 |= ADC_CR2_CONT;
}
/* Funcion que para la conversion ADC continua*/
void adc_StopContinuousConv(void) {
	ADC1->CR2 &= ~ADC_CR2_CONT;
}

/*Funcion que retora el ultimo dato adquirido por la ADC*/

uint16_t adc_GetValue(void) {
	return adcRawData;

}

/*esta es la ISR de la interacion por conversion ADC*/

void ADC_IRQHandler(void) {
	if (ADC1->SR & ADC_SR_EOC) {
		adcRawData = ADC1->DR;
		ADC1->SR &= ~ADC_SR_EOC;
	}
	adc_CompleteCallback();

}

__attribute__((weak)) void adc_CompleteCallback(void) {
	__NOP();
}

/*con esta funcion configuramos qué pin deseamos que funcione como ADC*/
void adc_ConfigAnalogPin(uint8_t adcChannel) {

	switch (adcChannel) {
	case CHANNEL_0:
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_0;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_1:
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_1;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_2:
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_2;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_3:
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_3;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_4:
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_4;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_5:
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_5;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_6:
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_6;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_7:
		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_7;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_8:
		handlerADCPin.pGPIOx = GPIOB;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_0;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_9:
		handlerADCPin.pGPIOx = GPIOB;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_1;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_10:
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_0;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_11:
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_1;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_12:
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_2;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_13:
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_3;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_14:
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_4;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_15:
		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_5;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;
	}
}

/*En esta funcion se configura para hacer conversiones en muchos canales y en un orden*/
void adc_ConfigMultichannel (ADC_Config_t *adcConfig, uint8_t numeroDeCanales){

	for (uint8_t i = 0; i <= numeroDeCanales;i++){
		/*Es necesario configurar un pin para que funcione como ADC */
		adc_ConfigAnalogPin(adcConfig[i].channel);
	}
	/*Se debe activar el RCC para el ADC1 con bus APB2*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	/*Luego siempre es necesario limpiar los registros*/
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;
	/*Se debe inactivar la forma de modo continuo por si esta activado*/
	adc_StopContinuousConv();
//	ADC1->CR2 &= ~ADC_CR2_CONT;
	/*Hay que activar el Scan*/
	adc_ScanMode(SCAN_ON);
//	ADC1->CR1 |= ADC_CR1_SCAN;
	/*Ahora hay que configurar el número de elementos que hay en las secuencias y configurar dicha secuencia*/
	ADC1->SQR1 |= (numeroDeCanales-1)<<ADC_SQR1_L_Pos;
	/*hay que poner el ciclo que va a ir recorriendo el arreglo a la vez que lo vaya configurando*/

	for (uint8_t i = 0; i <= numeroDeCanales;i++){
		/*Se va a hacer un switch case para la resolucion para cada elemento en el arreglo adcConfig*/
		switch (adcConfig[i].resolution){

		case RESOLUTION_12_BIT: {
			/*Cargar 0b00*/
			ADC1->CR1 &= ~ADC_CR1_RES;
			break;
		}
		case RESOLUTION_10_BIT: {
			/*Cargar 0b01*/
			ADC1->CR1 |= ADC_CR1_RES_0;
			break;
		}
		case RESOLUTION_8_BIT: {
			/*Cargar 0b10*/
			ADC1->CR1 |= ADC_CR1_RES_1;
			break;
		}
		case RESOLUTION_6_BIT: {
			/*Cargar 0b11*/
			ADC1->CR1 |= ADC_CR1_RES;
			break;
		}
		default: {
			/*Que sea 12 bit*/
			ADC1->CR1 &= ~ADC_CR1_RES;
			break;
		}

		}
		/*Ahora es necesario establecer la alineacion*/
		if (adcConfig[i].dataAlignment== ALIGNMENT_LEFT){
			/*Que se cargue alineacion a la izquierda*/
			ADC1->CR2 |= ADC_CR2_ALIGN;
		} else {
			/*Sino alineacion a la derecha*/
			ADC1->CR2 &= ~ADC_CR2_ALIGN;
		}
		/*Ahora a configurar el periodo de muestreo para cada elemento del arreglo*/
		/*Se hace para los 16 canales*/
		if (adcConfig[i].channel<CHANNEL_10){
			ADC1 -> SMPR2 |= (adcConfig[i].samplingPeriod <<(3*(adcConfig[i].channel)));
		}
		else{
			ADC1 -> SMPR1 |= (adcConfig[i].samplingPeriod<<(3*((adcConfig[i].channel)-10)));
		}
		/*Ahora se tiene que definir la secuencia */
		if(numeroDeCanales <=6){
			ADC1->SQR3 |= (adcConfig[i].channel << i*5);
		}
		else if((numeroDeCanales > 6) && (numeroDeCanales<=12) ){
			ADC1->SQR2 |= (adcConfig[i].channel << ((i*5)-30));
		}
		else if((numeroDeCanales > 12) && (numeroDeCanales<=16) ){
			ADC1->SQR1 |= (adcConfig[i].channel << ((i*5)-60));
		}
		else{
			__NOP();
		}
	}

		/*Ahora se configura el preescaler de 2:1*/
		ADC->CCR &= ~ADC_CCR_ADCPRE;
		/*Se desactivan las interrupciones globales */
		__disable_irq();
		/*Activamos la interrupcion debida a cuando se acabe una conversion*/
		ADC1->CR1 |= ADC_CR1_EOCIE;
		/*Activando lo que es la interrupcion cada que se termine cada conversion de canal*/
		ADC1->CR2|= ADC_CR2_EOCS;
		/*Matriculando la interrupción*/
		__NVIC_EnableIRQ(ADC_IRQn);
		/*Activacion del modulo ADC*/
		ADC1->CR2 |= ADC_CR2_ADON;
		/*Por ultimo activamos las interrupciones globales*/
		__enable_irq();
}


/*Se debe configurar el trigger externo*/
void adc_ConfigTrigger(uint8_t sourceType, Pwm_Handler_t *triggerSignal) {
	switch (sourceType) {
	    /*configuracion del conversor que siempre hace conversiones*/
	case TRIGGER_AUTO: {
		break;
	}
		/*configuracion que hará las conversiones de forma manual*/
	case TRIGGER_MANUAL:{
		break;
	}
		/*En este caso las conversiones se van a hacer con el PWM*/
	case TRIGGER_EXT: {

		/*Que sea con el flanco de subida*/
		ADC1->CR2 |= ADC_CR2_EXTEN_0;
		/*Ahora a configurar con el EXTSEL dependiendo del canal timer y canal PWM a utilizar*/
		/*Para timer 2*/
		if(triggerSignal->ptTIMx == TIM2){
	        switch(triggerSignal->config.Canal){
	        case channel_2_Pwm:{
	        	ADC1->CR2|=(3<<ADC_CR2_EXTSEL_Pos);
	        	break;
	        }
	        case channel_3_Pwm:{
	        	ADC1->CR2|=(4<<ADC_CR2_EXTSEL_Pos);
	        	break;
	        }
	        case channel_4_Pwm:{
	        	ADC1->CR2|=(5<<ADC_CR2_EXTSEL_Pos);
	        	break;
	        }
	        }
		}
		/*Para timer 3*/
		else if (triggerSignal->ptTIMx == TIM3){

	        	ADC1->CR2|=(7<<ADC_CR2_EXTSEL_Pos);

	        }
		/*Para el timer 4 solo hay un canal disponible*/
		else if (triggerSignal->ptTIMx == TIM4){

			ADC1->CR2|=(9<<ADC_CR2_EXTSEL_Pos);
	        }
		/*Para el timer 5*/
		else if (triggerSignal->ptTIMx == TIM5){

	        switch(triggerSignal->config.Canal){

	        case channel_1_Pwm:{
	        	ADC1->CR2|=(10<<ADC_CR2_EXTSEL_Pos);
	        	break;
	        }
	        case channel_2_Pwm:{
	        	ADC1->CR2|=(11<<ADC_CR2_EXTSEL_Pos);
	        	break;
	        }
	        case channel_3_Pwm:{
	        	ADC1->CR2|=(12<<ADC_CR2_EXTSEL_Pos);
	        	break;
	        }
	        }

	     }
	   }


   }
}



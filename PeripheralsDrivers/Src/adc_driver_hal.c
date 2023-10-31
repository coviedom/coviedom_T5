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

/* === Headers for private funcions === */
static void adc_enable_clock_peripheral(void);
static void adc_set_resolution(ADC_Config_t *adcConfig);
static void adc_set_alignment(ADC_Config_t *adcConfig);
static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig);
static void adc_set_one_channel_sequence(ADC_Config_t *adcConfig);
static void adc_config_interrupt(ADC_Config_t *adcConfig);

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
	switch (adcConfig->channel) {
	case CHANNEL_0: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*CArgar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP0;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_1: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP1_0 | ADC_SMPR2_SMP1_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP1;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_2: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP2_0 | ADC_SMPR2_SMP2_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP2_2 | ADC_SMPR2_SMP2_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP2_2 | ADC_SMPR2_SMP2_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP2;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_3: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP3_0 | ADC_SMPR2_SMP3_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP3_2 | ADC_SMPR2_SMP3_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP3_2 | ADC_SMPR2_SMP3_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP3;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_4: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP4_0 | ADC_SMPR2_SMP4_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP4_2 | ADC_SMPR2_SMP4_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP4_2 | ADC_SMPR2_SMP4_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP4;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_5: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP5_0 | ADC_SMPR2_SMP5_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP5_2 | ADC_SMPR2_SMP5_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP5_2 | ADC_SMPR2_SMP5_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP5;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_6: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP6_0 | ADC_SMPR2_SMP6_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP6_2 | ADC_SMPR2_SMP6_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP6_2 | ADC_SMPR2_SMP6_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP6;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_7: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP7_0 | ADC_SMPR2_SMP7_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP7_2 | ADC_SMPR2_SMP7_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP7_2 | ADC_SMPR2_SMP7_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP7;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_8: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP8_0 | ADC_SMPR2_SMP8_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP8_2 | ADC_SMPR2_SMP8_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP8_2 | ADC_SMPR2_SMP8_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP8;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_9: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP9_0 | ADC_SMPR2_SMP9_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP9_2 | ADC_SMPR2_SMP9_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR2 |= (ADC_SMPR2_SMP9_2 | ADC_SMPR2_SMP9_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR2 |= ADC_SMPR2_SMP9;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_10: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP10_0 | ADC_SMPR1_SMP10_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP10;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_11: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP11_0 | ADC_SMPR1_SMP11_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP11_2 | ADC_SMPR1_SMP11_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP11_2 | ADC_SMPR1_SMP11_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP11;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_12: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP12_0 | ADC_SMPR1_SMP12_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP12_2 | ADC_SMPR1_SMP12_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP12_2 | ADC_SMPR1_SMP12_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP12;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_13: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP13_0 | ADC_SMPR1_SMP13_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP13_2 | ADC_SMPR1_SMP13_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP13_2 | ADC_SMPR1_SMP13_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP13;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_14: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP14_0 | ADC_SMPR1_SMP14_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP14_2 | ADC_SMPR1_SMP14_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP14_2 | ADC_SMPR1_SMP14_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP14;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	case CHANNEL_15: {
		switch (adcConfig->samplingPeriod) {
		case SAMPLING_PERIOD_3_CYCLES:
			/*Cargar 0b000*/
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15;
			break;

		case SAMPLING_PERIOD_15_CYCLES:
			/*Cargar 0b001*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_0;
			break;

		case SAMPLING_PERIOD_28_CYCLES:
			/*Cargar 0b010*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_1;
			break;

		case SAMPLING_PERIOD_56_CYCLES:
			/*Cargar 0b011*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP15_0 | ADC_SMPR1_SMP15_1);
			break;

		case SAMPLING_PERIOD_84_CYCLES:
			/*Cargar 0b100*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_2;
			break;

		case SAMPLING_PERIOD_112_CYCLES:
			/*Cargar 0b101*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP15_2 | ADC_SMPR1_SMP15_0);
			break;

		case SAMPLING_PERIOD_144_CYCLES:
			/*Cargar 0b110*/
			ADC1->SMPR1 |= (ADC_SMPR1_SMP15_2 | ADC_SMPR1_SMP15_1);
			break;

		case SAMPLING_PERIOD_480_CYCLES:
			/*Cargar 0b111*/
			ADC1->SMPR1 |= ADC_SMPR1_SMP15;
			break;

		default: {
			__NOP();
			break;
		}
		}
		break;
	}
	default: {
		__NOP();
		break;
	}
	}
}
/*
 *  Configuramos el número de elementos en secuencia (solo un elemento)
 */
static void adc_set_one_channel_sequence(ADC_Config_t *adcConfig) {
	ADC1->SQR1 &= ~ADC_SQR1_L;

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
void adc_ConfigAnaloogPin(uint8_t adcChannel) {

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


/*
 * pwm_driver_hal.c
 *
 *  Created on: Noviembre 7, 2023
 *      Author: Cristhian Oviedo
 */

#include "stm32f4xx.h"
#include "pwm_driver_hal.h"
#include <stdint.h>
#include "timer_driver_hal.h"
#include <stm32_assert.h>

/* === Headers de las funciones privadas === */
static void activar_señal_reloj(PWM_Handler_t *pt_HandlerPwm);
static void establecer_la_frecuencia(PWM_Handler_t *pt_HandlerPwm);
static void establecer_ciclo_duty(PWM_Handler_t *pt_HandlerPwm);
static void preload_y_pwm(PWM_Handler_t *pt_HandlerPwm);

void configuracion_del_pwm(PWM_Handler_t *pt_HandlerPwm){
/*Funcion para activar la señal de reloj*/
	activar_señal_reloj(pt_HandlerPwm);
/*funcion que sirve para definir la frecuencia*/
	establecer_la_frecuencia(pt_HandlerPwm);
/*funcion para establecer el ciclo del duty*/
	establecer_ciclo_duty(pt_HandlerPwm);

	pt_HandlerPwm->pTIMx->CR1  &= ~TIM_CR1_DIR;

	pt_HandlerPwm->pTIMx->CNT = 0;
/*cargamos el preload y tambien configuracion del pwm*/
	preload_y_pwm(pt_HandlerPwm);
/*se activa la salida*/
	activar_salida(pt_HandlerPwm);

	}

static void activar_señal_reloj(PWM_Handler_t *pt_HandlerPwm){

	if(pt_HandlerPwm->pTIMx == TIM2){

		RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(pt_HandlerPwm->pTIMx == TIM3){

		RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(pt_HandlerPwm->pTIMx == TIM4){
		RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(pt_HandlerPwm->pTIMx == TIM5){
		RCC->APB1ENR &= ~RCC_APB1ENR_TIM5EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}
}

static void establecer_la_frecuencia(PWM_Handler_t *pt_HandlerPwm){

    pt_HandlerPwm->pTIMx->PSC = pt_HandlerPwm->config.prescaler-1;

	pt_HandlerPwm->pTIMx->ARR = pt_HandlerPwm->config.periodo-1;

}
static void establecer_ciclo_duty(PWM_Handler_t *pt_HandlerPwm){
	switch(pt_HandlerPwm->config.Canal){
	case PWM_CHANNEL_1:{
		pt_HandlerPwm->pTIMx->CCR1 = (pt_HandlerPwm->config.CicloDuty);
		break;
	}
	case PWM_CHANNEL_2: {
		pt_HandlerPwm->pTIMx->CCR2 = (pt_HandlerPwm->config.CicloDuty);
		break;
	}
	case PWM_CHANNEL_3: {
		pt_HandlerPwm->pTIMx->CCR3 = (pt_HandlerPwm->config.CicloDuty);
		break;
	}
	case PWM_CHANNEL_4: {
		pt_HandlerPwm->pTIMx->CCR4 = (pt_HandlerPwm->config.CicloDuty);
		break;
	}
	default:{
		__NOP();
		break;
	}
}
}

static void preload_y_pwm(PWM_Handler_t *pt_HandlerPwm){
	switch(pt_HandlerPwm->config.Canal){
	case PWM_CHANNEL_1:{
		pt_HandlerPwm->pTIMx->CCMR1 &= ~TIM_CCMR1_CC1S;

		pt_HandlerPwm->pTIMx->CCMR1 |= TIM_CCMR1_OC1FE;

		pt_HandlerPwm->pTIMx->CCMR1 |= TIM_CCMR1_OC1M_1;

		pt_HandlerPwm->pTIMx->CCMR1 |= TIM_CCMR1_OC1M_2;

		pt_HandlerPwm->pTIMx->CCMR1 |= TIM_CCMR1_OC1PE;
		break;
	}
	case PWM_CHANNEL_2:{
		pt_HandlerPwm->pTIMx->CCMR1 &= ~TIM_CCMR1_CC2S;
		pt_HandlerPwm->pTIMx->CCMR1 |= TIM_CCMR1_OC2FE;
		pt_HandlerPwm->pTIMx->CCMR1 |= TIM_CCMR1_OC2M_1;
		pt_HandlerPwm->pTIMx->CCMR1 |= TIM_CCMR1_OC2M_2;
		pt_HandlerPwm->pTIMx->CCMR1 |= TIM_CCMR1_OC2PE;
		break;
	}
	case PWM_CHANNEL_3: {

		pt_HandlerPwm->pTIMx->CCMR2 &= ~TIM_CCMR2_CC3S;;

		pt_HandlerPwm->pTIMx->CCMR2 |= TIM_CCMR2_OC3FE;

		pt_HandlerPwm->pTIMx->CCMR2 |= TIM_CCMR2_OC3M_1;

		pt_HandlerPwm->pTIMx->CCMR2 |= TIM_CCMR2_OC3M_2;

		pt_HandlerPwm->pTIMx->CCMR2 |= TIM_CCMR2_OC3PE;
		break;
	}
	case PWM_CHANNEL_4: {

		pt_HandlerPwm->pTIMx->CCMR2 &= ~TIM_CCMR2_CC4S;;

		pt_HandlerPwm->pTIMx->CCMR2 |= TIM_CCMR2_OC4FE;

		pt_HandlerPwm->pTIMx->CCMR2 |= TIM_CCMR2_OC4M_1;

		pt_HandlerPwm->pTIMx->CCMR2 |= TIM_CCMR2_OC4M_2;

		pt_HandlerPwm->pTIMx->CCMR2 |= TIM_CCMR2_OC4PE;
		break;
	}
	default:{
		__NOP();
		break;
	}
}
}
/*funcion para que inicie la señal*/
void inicio_de_señal_pwm(PWM_Handler_t *pt_HandlerPwm) {

	pt_HandlerPwm->pTIMx->CR1 |= TIM_CR1_ARPE;

	pt_HandlerPwm->pTIMx->CR1 |= TIM_CR1_CEN;
}
/*funcion para frenar la señal del pwm*/
void frenar_señal_pwm(PWM_Handler_t *pt_HandlerPwm) {
	pt_HandlerPwm->pTIMx->CR1 &= ~TIM_CR1_ARPE;

	pt_HandlerPwm->pTIMx->CR1 &= ~TIM_CR1_CEN;
}

/*funcion para actualizar la frecuencia*/
void actualiza_frecuencia(PWM_Handler_t *pt_HandlerPwm, uint16_t frecuencia_nueva){

	pt_HandlerPwm->config.periodo = frecuencia_nueva;

	establecer_la_frecuencia(&*pt_HandlerPwm);
}
/*funcion para actualizar el ciclo del duty*/
void actualiza_Ciclo_Duty(PWM_Handler_t *pt_HandlerPwm, uint16_t Duty_nuevo){

	pt_HandlerPwm->config.CicloDuty = Duty_nuevo-1;

	establecer_ciclo_duty(&*pt_HandlerPwm);
}
/*se activa la salida*/
void activar_salida(PWM_Handler_t *pt_HandlerPwm) {
	switch (pt_HandlerPwm->config.Canal) {
	case PWM_CHANNEL_1: {

		pt_HandlerPwm->pTIMx->CCER |= TIM_CCER_CC1E;
		break;
	}
	case PWM_CHANNEL_2: {
		pt_HandlerPwm->pTIMx->CCER |= TIM_CCER_CC2E;
		break;
	}
	case PWM_CHANNEL_3: {
		pt_HandlerPwm->pTIMx->CCER |= TIM_CCER_CC3E;
		break;
	}
	case PWM_CHANNEL_4: {
		pt_HandlerPwm->pTIMx->CCER |= TIM_CCER_CC4E;
		break;
	}
	default: {
		__NOP();
		break;
	}
	}
}



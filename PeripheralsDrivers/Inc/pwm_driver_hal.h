/*
 * pwm_driver_hal.h
 *
 *  Created on: Noviembre 7, 2023
 *      Author: Cristhian Oviedo
 */
#ifndef PWM_DRIVER_HAL_H_
#define PWM_DRIVER_HAL_H_

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>

/*definimos los canales*/
enum
{
	channel_1_Pwm = 0,
	channel_2_Pwm,
	channel_3_Pwm,
	channel_4_Pwm
};

/*Estructura de configuracion*/
typedef struct
{
	uint8_t		Canal; 		    /*Canal del PWM */
	uint32_t	prescaler;		/*valor de incremento*/
	uint16_t	periodo;		/*Este va relacionado con el prescaler*/
	uint16_t	CicloDuty;		/*Valor del tiempo que la señal se encuentra en alto*/
}Pwm_Configura_t;

/* Handler para el PWM*/
typedef struct
{
	TIM_TypeDef		*ptTIMx;
	Pwm_Configura_t	config;	/*Configuración inicial*/
}Pwm_Handler_t;

/*Header definiciones para las funciones publicas del pwm_driver_hal.h*/
void configuracion_del_pwm(Pwm_Handler_t *pt_HandlerPwm);
void actualiza_frecuencia(Pwm_Handler_t *pt_HandlerPwm, uint16_t frecuencia_nueva);
void actualiza_Ciclo_Duty(Pwm_Handler_t *pt_HandlerPwm, uint16_t Duty_nuevo);
void inicio_de_señal_pwm(Pwm_Handler_t *pt_HandlerPwm);
void frenar_señal_pwm(Pwm_Handler_t *pt_HandlerPwm);
void activar_salida(Pwm_Handler_t *pt_HandlerPwm);


#endif /* PWM_DRIVER_HAL_H_ */


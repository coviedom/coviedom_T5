/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Cristhian Andrés Oviedo Montalvo
 * @brief          : Solución de la Tarea 4 de Taller 5
 * Fecha           : 18 de Noviembre 2023
 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stm32f4xx.h>
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "exti_driver_hal.h"
#include "pwm_driver_hal.h"
#include "timer_driver_hal.h"
#include "arm_math.h"

#define CANTIDAD_DE_SENSORES 3

/*Se encabezan las funciones a utilizar en el programa*/
void start(void);


/*Se define el Led zul del blinky*/
GPIO_Handler_t led_Blinky = {0};           //PB10

/*Se definen los timer a utilizar*/
Timer_Handler_t timer_del_Blinky = {0};

/*Se define el Usart y sus respectivos pines*/
GPIO_Handler_t Transmisor = {0};   //PA2
GPIO_Handler_t Receptor = {0};     //PA3
USART_Handler_t usart2 = {0};

/*Se define los tres tipo ADC_Config_t*/
ADC_Config_t _Sensor1 = {0};
ADC_Config_t _Sensor2 = {0};
ADC_Config_t _Sensor3 = {0};
/*Se crea un arreglo que va a contener los sensores*/
ADC_Config_t _Sensores[CANTIDAD_DE_SENSORES]= {0};

/*Se crea el PWM que va ayudar a muestrear las señales*/
Pwm_Handler_t _PWM_Muestreo = {0};

/*variable que guarda el caracter recibido tras presionar una tecla*/
uint8_t teclado = 0;
/*Se crea una variable para guardar las conversiones que se hace en cada canal*/
uint8_t _Dato_de_Secuencia = 0;
/*Definimos el arreglo en el que se guardará el mensaje para cool Term*/
char buffer_info[128] = {0};


int main(void) {

	/*Se activa el co-procesador FPU*/
	SCB->CPACR |= (0b11 << 20);

	/*funcion que se encarga de configurar las definiciones antes mencionadas*/
	start();

	/*Lo que realiza el codigo ciclicamente*/
	while (1) {


	}









/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Cristhian Andrés Oviedo Montalvo
 * @brief          : Solución de la Tarea 3 de Taller 5
 * Fecha           : 29 de octubre 2023
 ******************************************************************************
 */
#include <stdint.h>
#include <stm32f4xx.h>
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "exti_driver_hal.h"
#include "timer_driver_hal.h"

/*Se encabezan las funciones a utilizar en el programa*/
void start(void);
void mostrar_resultados_test(uint8_t number_resolucion);
void mostrar_resolucion_elegida(uint8_t num_resolu);
void mostrar_sensor_elegido(uint8_t num_sensor);
void cambiador_de_canal_sensor(ADC_Config_t *_ADC, uint16_t _Canal);
void cambiador_de_resolucion(ADC_Config_t *adc, uint16_t resoluc);
void enviarInfo_al_7_segmentos(void);
void establecer_resolucion_en_pantalla(uint8_t resolucion);
void establecer_sensor_en_pantalla(uint8_t sensor);

/*Se define el Led zul del blinky*/
GPIO_Handler_t led_Blinky = {0};              //PB10

/*Se definen los segmentos del display*/
GPIO_Handler_t puntico = {0};                 //PC6
GPIO_Handler_t segmento_A = {0};   		      //PC12
GPIO_Handler_t segmento_B = {0};   		      //PC11
GPIO_Handler_t segmento_C = {0};   		      //PC5
GPIO_Handler_t segmento_D = {0};   		      //PC9
GPIO_Handler_t segmento_E = {0};   		      //PC8
GPIO_Handler_t segmento_G = {0};   		      //PB9
GPIO_Handler_t transistor_sensores = {0};     //PB6
GPIO_Handler_t transistor_resoluciones = {0}; //PB8

/*Se definen los timer a utilizar*/
Timer_Handler_t timer_del_Blinky = {0};
Timer_Handler_t timer_Display = {0};
Timer_Handler_t timer_refresh = {0}; /*Este es el timer que actualiza los datos del test ADC*/

/*Definicion de los pines e interrupciones para lo relacionado al encoder*/
EXTI_Config_t exti_del_clock = {0};           //Exti 3
GPIO_Handler_t clock_del_encoder = {0};	      //PC3
GPIO_Handler_t _Data = {0};                   //PC1
EXTI_Config_t exti_del_modo = {0};            //Exti 5
GPIO_Handler_t boton_modo = {0};	          //PB5

/*Se define el Usart y sus respectivos pines*/
GPIO_Handler_t Transmisor = {0};
GPIO_Handler_t Receptor = {0};
USART_Handler_t usart1 = {0};

/*Se define para el ADC*/
ADC_Config_t _Sensor = {0};
/* Canal 5 -> Sensor 3
 * Canal 6 -> Sensor 2
 * Canal 7 -> Sensor 1
 */

/*Se definen los contadores a utilizar en el programa*/
int8_t contador_sensor = 1;
int8_t contador_resolucion = 0;

/*Se definen las banderas a utilizar en el programa*/
uint8_t bandera_modo = 0;     /*Bandera cuando se presiona el boton o la letra "m"*/
uint8_t bandera_giro = 0;     /*Bandera cuando se gira el encoder*/
uint8_t bandera_adc = 0;      /*Bandera cuando se lanza una conversion ADC*/
uint8_t bandera_refresh = 0;  /*Bandera cuando el timer esta encendido*/
uint8_t teclado = 0;          /*variable que guarda el caracter recibido tras presionar una tecla*/

/*Definimos el arreglo en el que se guardará el mensaje*/
char buffer_info[128] = {0};

/*Este enum nos ayudará a leer el codigo de mejor manera*/
enum{
	Sensor_1 = 1,
	Sensor_2,
	Sensor_3,
};
/*Este enum nos ayudará a leer el codigo de mejor manera*/
enum{
	Resolucion_6_Bit = 0,
	Resolucion_8_Bit,
	Resolucion_10_Bit,
	Resolucion_12_Bit
};

int main(void) {

	/*funcion que se encarga de configurar las definiciones antes mencionadas*/
	start();
	/*Lo que realiza el codigo ciclicamente*/
	while (1) {
		/*Si se presiona "j" se enciende el timer de actualizacion de ADC*/
		if (teclado == 'j') {
			usart_writeMsg(&usart1, "Test de ADC encendido ! +++++++++++++++++++++++++++++++++++ \n\r");
			timer_SetState(&timer_refresh, TIMER_ON);
			teclado = 0;
		}
		/*Si el timer de actualizacion de ADC esta endendido ocurrirá lo que sigue*/
		if (bandera_refresh == SET){
			/*Si se hace una conversion simple de ADC entonces ocurrirá lo que sigue */
			if (bandera_adc) {
				bandera_adc = 0;
				/*Funcion que permite mostrar el sensor que se está testeando, la resolucion y tambien el voltaje equivalente en Voltios*/
				mostrar_resultados_test(contador_resolucion);
			}
			/*Si lo que recibe la variable teclado es diferente del caracter nulo entonces ocurrirá lo que sigue*/
			if (teclado != '\0') {
				/*Si se presina la letra "p" entonces se muestra un menu de ayuda para los comandos que se pueden utilizar */
				if (teclado == 'p') {
					usart_writeMsg(&usart1, "Menu de ayuda:\n j -> Enciende el test de ADC  \n k ->  Detiene el test de ADC \n a -> Aumenta \n d -> Disminuye \n m -> Cambia de modo \n Nota: Por defecto el test esta encendido\n \r");
					teclado = 0;
				}
				/*Si se presiona "j" se enciende el timer de actualizacion de ADC*/
				else if (teclado == 'j') {
					usart_writeMsg(&usart1, "Test de ADC encendido ! +++++++++++++++++++++++++++++++++++ \n\r");
					timer_SetState(&timer_refresh, TIMER_ON);
					teclado = 0;
				}
				/*Si se presiona "k" se apaga el timer de actualiacion de ADC*/
				else if (teclado == 'k') {
					usart_writeMsg(&usart1, "Se acaba de detener el test de ADC ! ++++++++++++++++++++++\n\r");
					timer_SetState(&timer_refresh, TIMER_OFF);
					bandera_refresh = RESET;
					teclado = 0;
				}
				/*Si se presiona "m" y está en el modo sensores entonces se cambia la bandera para pasar a resoluciones*/
				else if (teclado == 'm'&& bandera_modo == SET ) {
					usart_writeMsg(&usart1, "¡Cambiando a elegir Resoluciones! *************************\n\r");
					bandera_modo ^= 1;
					teclado = 0;
				}
				/*Si se presiona "m" y está en el modo resoluciones entonces se cambia la bandera para pasar a sensores*/
				else if (teclado == 'm'&& bandera_modo == RESET ) {
					usart_writeMsg(&usart1, "¡Cambiando a elegir Sensores! *****************************\n\r");
					bandera_modo ^= 1;
					teclado = 0;
				}
				/*Si se presiona la letra "a" ocurrirá lo que sigue*/
				else if (teclado == 'a') {
					/*Si está en el modo sensores entonces ocurrirá lo que sigue */
					if (bandera_modo == SET) {
						/*El contador del sensor se incrementa en una unidad */
						contador_sensor++;
						/*Se establece un limite superior*/
						if (contador_sensor > 3) {
							contador_sensor = 3;
						}
						/*Funcion que muestra el sensor que se elije*/
						mostrar_sensor_elegido(contador_sensor);
						teclado = 0;
					}
					/*Si está en el modo sensores entonces ocurrirá lo que sigue */
					else if (bandera_modo == RESET) {
						/*El contador de las resoluciones se incrementa en una unidad */
						contador_resolucion++;
						/*Se establece un limite superior*/
						if (contador_resolucion > 3) {
							contador_resolucion = 3;
						}
						/*Funcion que muestra la resolucion que se elije*/
						mostrar_resolucion_elegida(contador_resolucion);
						teclado = 0;
					}
				 }
				/*Si se presiona la letra "a" ocurrirá lo que sigue*/
				else if (teclado == 'd') {
					/*Si está en el modo sensores entonces ocurrirá lo que sigue */
					if (bandera_modo == SET) {
						/*El contador del sensor se dismnuirá en una unidad */
						contador_sensor--;
						/*Se establece un limite inferior*/
						if (contador_sensor < 1) {
							contador_sensor = 1;
						}
						/*Funcion que muestra el sensor que se elije*/
						mostrar_sensor_elegido(contador_sensor);
						teclado = 0;
					}
					/*Si está en el modo resoluciones entonces ocurrirá lo que sigue */
					else if (bandera_modo == RESET) {
						/*El contador de la resolucion se dismnuirá en una unidad */
						contador_resolucion--;
						/*Se establece un limite inferior*/
						if (contador_resolucion < 0) {
							contador_resolucion = 0;
						}
						/*Funcion que muestra la resolucion que se elije*/
						mostrar_resolucion_elegida(contador_resolucion);
						teclado = 0;
					}
				}
				else {
					__NOP();
				}
			}
			/*Lo siguiente tendrá la misma funcionalidad que lo anterior pero por interaccion con el encoder
			 * primero se evalua si se ha girado el encoder*/
			if (bandera_giro == SET) {
				/*Se baja la bandera*/
				bandera_giro = RESET;
				/*Se lee el estado del data encoder*/
				uint32_t _Lectura = gpio_ReadPin(&_Data);
				/*Si está en el modo sensores y el data es 1, osea giro a la derecha entonces ocurrirá lo que sigue*/
				if (bandera_modo == SET && _Lectura == SET) {
					/*Se incrementa el contador del sensor*/
					contador_sensor++;
					/*Se establece un limite superior*/
					if (contador_sensor > 3) {
						contador_sensor = 3;
					}
					/*Funcion que muestra el sensor que se elije*/
					mostrar_sensor_elegido(contador_sensor);
				}
				/*Si está en el modo sensores y el data es 0, osea giro a la izquierda entonces ocurrirá lo que sigue*/
				else if (bandera_modo == SET && _Lectura == RESET) {
					/*Se disminuye el contador del sensor*/
					contador_sensor--;
					/*Se establece un limite inferiorr*/
					if (contador_sensor < 1) {
						contador_sensor = 1;
					}
					/*Funcion que muestra el sensor que se elije*/
					mostrar_sensor_elegido(contador_sensor);
				}
				/*Si está en el modo resoluciones y el data es 1, osea giro a la derecha entonces ocurrirá lo que sigue*/
				else if (bandera_modo == RESET && _Lectura == SET) {
					/*Se aumenta el contador de la resolucion*/
					contador_resolucion++;
					/*Se establece un limite superior*/
					if (contador_resolucion > 3) {
						contador_resolucion = 3;
					}
					/*Funcion que muestra la resolucion que se elije*/
					mostrar_resolucion_elegida(contador_resolucion);
				}
				/*Si está en el modo resoluciones y el data es 0, osea giro a la izquierda entonces ocurrirá lo que sigue*/
				else if (bandera_modo == RESET && _Lectura == RESET) {
					contador_resolucion--;
					if (contador_resolucion < 0) {
						contador_resolucion = 0;
					}
					/*Funcion que muestra la resolucion que se elije*/
					mostrar_resolucion_elegida(contador_resolucion);
				}
				else {
					__NOP();
				}
				enviarInfo_al_7_segmentos();
			}
		}
	}
}
void start(void) {
	/*Se configura el punto del display*/
	puntico.pGPIOx = GPIOC;
	puntico.pinConfig.GPIO_PinNumber = PIN_6;
	puntico.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	puntico.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	puntico.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	puntico.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&puntico);
	/*Se configura el segmento A del display*/
	segmento_A.pGPIOx = GPIOC;
	segmento_A.pinConfig.GPIO_PinNumber = PIN_12;
	segmento_A.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_A.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_A.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_A.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_A);
	/*Se configura el segmento B del display*/
	segmento_B.pGPIOx = GPIOC;
	segmento_B.pinConfig.GPIO_PinNumber = PIN_11;
	segmento_B.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_B.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_B.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_B.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_B);
	/*Se configura el segmento C del display*/
	segmento_C.pGPIOx = GPIOC;
	segmento_C.pinConfig.GPIO_PinNumber = PIN_5;
	segmento_C.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_C.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_C.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_C.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_C);
	/*Se configura el segmento D del display*/
	segmento_D.pGPIOx = GPIOC;
	segmento_D.pinConfig.GPIO_PinNumber = PIN_9;
	segmento_D.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_D.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_D.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_D.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_D);
	/*Se configura el segmento E del display*/
	segmento_E.pGPIOx = GPIOC;
	segmento_E.pinConfig.GPIO_PinNumber = PIN_8;
	segmento_E.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_E.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_E.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_E.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_E);
	/*Se configura el segmento G del display*/
	segmento_G.pGPIOx = GPIOB;
	segmento_G.pinConfig.GPIO_PinNumber = PIN_9;
	segmento_G.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	segmento_G.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	segmento_G.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	segmento_G.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&segmento_G);
	/*Se configura el led azul del blinky*/
	led_Blinky.pGPIOx = GPIOB;
	led_Blinky.pinConfig.GPIO_PinNumber = PIN_10;
	led_Blinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	led_Blinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	led_Blinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	led_Blinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&led_Blinky);
	/*Se configura pin del display tipo catodo comun*/
	transistor_sensores.pGPIOx = GPIOB;
	transistor_sensores.pinConfig.GPIO_PinNumber = PIN_6;
	transistor_sensores.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	transistor_sensores.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	transistor_sensores.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	transistor_sensores.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&transistor_sensores);
	/*Se configura pin del display tipo catodo comun*/
	transistor_resoluciones.pGPIOx = GPIOB;
	transistor_resoluciones.pinConfig.GPIO_PinNumber = PIN_8;
	transistor_resoluciones.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	transistor_resoluciones.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	transistor_resoluciones.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	transistor_resoluciones.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&transistor_resoluciones);
	/*Se configura el pin relacionado al Data del encoder */
	_Data.pGPIOx = GPIOC;
	_Data.pinConfig.GPIO_PinNumber = PIN_1;
	_Data.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	_Data.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Data.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_Data.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_Data);
	/*Se configura el pin relacionado al Clock del encoder */
	clock_del_encoder.pGPIOx = GPIOC;
	clock_del_encoder.pinConfig.GPIO_PinNumber = PIN_3;
	clock_del_encoder.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	clock_del_encoder.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	clock_del_encoder.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	clock_del_encoder.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	/*Se configura el exti relacionado a la interrupcion por el clock de giro del encoder*/
	exti_del_clock.pGPIOHandler = &clock_del_encoder;
	exti_del_clock.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	exti_Config(&exti_del_clock);
	/*Se configura el SW del encoder*/
	boton_modo.pGPIOx = GPIOB;
	boton_modo.pinConfig.GPIO_PinNumber = PIN_5;
	boton_modo.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	boton_modo.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	boton_modo.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	boton_modo.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	/*Se configura el exti relacionado a la interrupcion por el SW del encoder*/
	exti_del_modo.pGPIOHandler = &boton_modo;
	exti_del_modo.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	exti_Config(&exti_del_modo);
	/*Se congura el usart de comunicacion serial*/
	usart1.ptrUSARTx = USART1;
	usart1.USART_Config.baudrate = USART_BAUDRATE_9600;
	usart1.USART_Config.datasize = USART_DATASIZE_8BIT;
	usart1.USART_Config.parity = USART_PARITY_NONE;
	usart1.USART_Config.stopbits = USART_STOPBIT_1;
	usart1.USART_Config.mode = USART_MODE_RXTX;
	usart1.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart_Config(&usart1);
	/*Se configura el pin transmisor*/
	Transmisor.pGPIOx = GPIOA;
	Transmisor.pinConfig.GPIO_PinNumber = PIN_9;
	Transmisor.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Transmisor.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Transmisor.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	Transmisor.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Transmisor.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&Transmisor);
	/*Se configura el pin receptor*/
	Receptor.pGPIOx = GPIOA;
	Receptor.pinConfig.GPIO_PinNumber = PIN_10;
	Receptor.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Receptor.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Receptor.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	Receptor.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Receptor.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&Receptor);
	/*Se configura el ADC*/
	_Sensor.channel = CHANNEL_7;
	_Sensor.resolution = RESOLUTION_6_BIT;
	_Sensor.dataAlignment = ALIGNMENT_RIGHT;
	_Sensor.samplingPeriod = SAMPLING_PERIOD_84_CYCLES;
	_Sensor.interrupState = ADC_INT_ENABLE;
	adc_ConfigSingleChannel(&_Sensor);
	/*Se configura el timer para el blinky*/
	timer_del_Blinky.pTIMx = TIM3;
	timer_del_Blinky.TIMx_Config.TIMx_Prescaler = 16000;
	timer_del_Blinky.TIMx_Config.TIMx_Period = 250;
	timer_del_Blinky.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	timer_del_Blinky.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&timer_del_Blinky);
	/*que el timer esté encendido*/
	timer_SetState(&timer_del_Blinky, TIMER_ON);
	/*Se configura el timer para el display a alta frecuencia*/
	timer_Display.pTIMx = TIM5;
	timer_Display.TIMx_Config.TIMx_Prescaler = 16000;
	timer_Display.TIMx_Config.TIMx_Period = 5;
	timer_Display.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	timer_Display.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&timer_Display);
	/*que el timer esté encendido*/
	timer_SetState(&timer_Display, TIMER_ON);
	/*Se configura el timer para la actualizacion de datos con ARR = 5 Segundos*/
	timer_refresh.pTIMx = TIM4;
	timer_refresh.TIMx_Config.TIMx_Prescaler = 16000;
	timer_refresh.TIMx_Config.TIMx_Period = 5000; // ARR = 5[s]
	timer_refresh.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	timer_refresh.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&timer_refresh);
	timer_SetState(&timer_refresh, TIMER_ON);
	/*Se establecen en diferentes estados para que prendan de forma alternante*/
	gpio_WritePin(&transistor_sensores, SET);
	gpio_WritePin(&transistor_resoluciones, RESET);
}
/*Funcion que permite mostrar el sensor que se está testeando, la resolucion y tambien el voltaje equivalente en Voltios*/
void mostrar_resultados_test(uint8_t number_resolucion) {

	switch (number_resolucion) {
	    case Resolucion_6_Bit:
	        sprintf(buffer_info, "ADC = %d  = %.2f[V]   | Sensor: %d con Resolución: 6 Bit|\n\r", _Sensor.adcData, _Sensor.adcData * (float)(3.3 / 63), contador_sensor);
	        usart_writeMsg(&usart1, buffer_info);
	        break;
	    case Resolucion_8_Bit:
	        sprintf(buffer_info, "ADC = %d  = %.2f[V]   | Sensor: %d con Resolución: 8 Bit |\n\r", _Sensor.adcData, _Sensor.adcData * (float)(3.3 / 255), contador_sensor);
	        usart_writeMsg(&usart1, buffer_info);
	        break;
	    case Resolucion_10_Bit:
	        sprintf(buffer_info, "ADC = %d  = %.2f[V]   | Sensor: %d con Resolución: 10 Bit|\n\r", _Sensor.adcData, _Sensor.adcData * (float)(3.3 / 1023), contador_sensor);
	        usart_writeMsg(&usart1, buffer_info);
	        break;
	    case Resolucion_12_Bit:
	        sprintf(buffer_info, "ADC = %d  = %.2f[V]   | Sensor: %d con Resolución: 12 Bit |\n\r", _Sensor.adcData, _Sensor.adcData * (float)(3.3 / 4095), contador_sensor);
	        usart_writeMsg(&usart1, buffer_info);
	        break;
	    default:
	        __NOP();
	}
}
/*Funcion que muestra la resolucion que se elije*/
void mostrar_resolucion_elegida(uint8_t num_resolu) {
	switch (num_resolu) {
	    case Resolucion_6_Bit:
	        sprintf(buffer_info, "Resolución = 6 Bit   ______________________________________\n\r");
	        usart_writeMsg(&usart1, buffer_info);
	        /*Funcion encargada de cambiar la resolucion*/
	        cambiador_de_resolucion(&_Sensor, RESOLUTION_6_BIT);
	        break;
	    case Resolucion_8_Bit:
	        sprintf(buffer_info, "Resolución = 8 Bit   ______________________________________\n\r");
	        usart_writeMsg(&usart1, buffer_info);
	        cambiador_de_resolucion(&_Sensor, RESOLUTION_8_BIT);
	        break;
	    case Resolucion_10_Bit:
	        sprintf(buffer_info, "Resolución = 10 Bit  ______________________________________\n\r");
	        usart_writeMsg(&usart1, buffer_info);
	        cambiador_de_resolucion(&_Sensor, RESOLUTION_10_BIT);
	        break;
	    case Resolucion_12_Bit:
	        sprintf(buffer_info, "Resolución = 12 Bit  ______________________________________\n\r");
	        usart_writeMsg(&usart1, buffer_info);
	        cambiador_de_resolucion(&_Sensor, RESOLUTION_12_BIT);
	        break;
	    default:
	        __NOP();
	        break;
	}
}
/*Funcion que muestra el sensor que se elije*/
void mostrar_sensor_elegido(uint8_t num_sensor) {

	sprintf(buffer_info, "Sensor = %d  _______________________________________________\n\r", contador_sensor);
	usart_writeMsg(&usart1, buffer_info);

	switch (num_sensor) {
	    case Sensor_1:
	    	/*Funcion que se encarga de cambiar el canal de ADC */
	        cambiador_de_canal_sensor(&_Sensor, CHANNEL_7);
	        break;
	    case Sensor_2:
	        cambiador_de_canal_sensor(&_Sensor, CHANNEL_6);
	        break;
	    case Sensor_3:
	        cambiador_de_canal_sensor(&_Sensor, CHANNEL_5);
	        break;
	    default:
	        __NOP();
	        break;
	}
}
/*Funcion que se encarga de cambiar el canal de ADC */
void cambiador_de_canal_sensor(ADC_Config_t *_ADC, uint16_t _Canal) {
	_ADC->channel = _Canal;
	adc_ConfigSingleChannel(_ADC);
}
/*Funcion encargada de cambiar la resolucion*/
void  cambiador_de_resolucion(ADC_Config_t *adc, uint16_t resoluc) {
	adc->resolution = resoluc;
	adc_ConfigSingleChannel(adc);
}
/*Funcion que se encarga de enviar la informacion que tiene al display*/
void enviarInfo_al_7_segmentos(void) {
	/*Se lee el estado del transistor*/
	uint32_t read1 = gpio_ReadPin(&transistor_sensores);
	/*Entonces si está en 1 es por que esta encendido el de las resoluciones ya que es de catodo comun*/
	if (read1 == SET) {
		/*Funcion encargada de prender los segmentos necesarios en el display para mostrar la resolucion segun sea el contador*/
		establecer_resolucion_en_pantalla(contador_resolucion);
		/*Si está en el modo sensores entonces ocurrirá lo que sigue */
		if (bandera_modo == SET) {
			/*Se enciende puntico del display del lado de los sensores*/
			gpio_WritePin(&puntico, SET);
		}
	}
	/*Se lee el estado del transistor*/
	uint32_t read2 = gpio_ReadPin(&transistor_resoluciones);
	/*Entonces si está en 1 es por que esta encendido el de los sensores ya que es de catodo comun*/
	if (read2 == SET) {
		/*Funcion encargada de prender los segmentos necesarios en el display para mostrar el sensor segun sea el contador*/
		establecer_sensor_en_pantalla(contador_sensor);
		/*Si está en el modo resoluciones entonces ocurrirá lo que sigue */
		if (bandera_modo == RESET) {
			/*Se enciende puntico del display del lado de las resoluciones*/
			gpio_WritePin(&puntico, SET);
		}
	}
}
/*Funcion encargada de prender los segmentos necesarios en el display para mostrar la resolucion segun sea el contador*/
void establecer_resolucion_en_pantalla(uint8_t resolucion) {

	switch (resolucion) {

	case Resolucion_6_Bit:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, SET);
		gpio_WritePin(&segmento_B, SET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, SET);
		gpio_WritePin(&segmento_E, RESET);
		gpio_WritePin(&segmento_G, SET);
		break;

	case Resolucion_8_Bit:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, SET);
		gpio_WritePin(&segmento_B, SET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, SET);
		break;

	case Resolucion_10_Bit:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, SET);
		gpio_WritePin(&segmento_B, SET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, RESET);
		break;

	case Resolucion_12_Bit:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, RESET);
		gpio_WritePin(&segmento_B, SET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, RESET);
		break;

	default: {
		__NOP();

		break;
	}
	}
}
/*Funcion encargada de prender los segmentos necesarios en el display para mostrar el sensor segun sea el contador*/
void establecer_sensor_en_pantalla(uint8_t sensor) {

	switch (sensor) {

	case Sensor_1:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, SET);
		gpio_WritePin(&segmento_B, RESET);
		gpio_WritePin(&segmento_C, RESET);
		gpio_WritePin(&segmento_D, SET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, SET);
		break;

	case Sensor_2:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, RESET);
		gpio_WritePin(&segmento_B, RESET);
		gpio_WritePin(&segmento_C, SET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, RESET);
		gpio_WritePin(&segmento_G, RESET);
		break;

	case Sensor_3:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&segmento_A, RESET);
		gpio_WritePin(&segmento_B, RESET);
		gpio_WritePin(&segmento_C, RESET);
		gpio_WritePin(&segmento_D, RESET);
		gpio_WritePin(&segmento_E, SET);
		gpio_WritePin(&segmento_G, RESET);
		break;

	default: {
		__NOP();
		break;
	}
	}
}
/*Fuuncion que si se lanza una conversion analogica digital entonces se sube una bandera y se obtiene el valor */
void adc_CompleteCallback(void) {
	bandera_adc = SET;
	_Sensor.adcData = adc_GetValue();
}
/*Funcion que si se recibe algo por comunicacion serial almacena la informacion en la variable teclado*/
void usart1_RxCallback(void) {
	teclado = usart_getRxData();
}
/*Con esta funcion de interrupcion se cambia el estado del led dependiendo del ARR*/
void Timer3_Callback(void) {
	gpio_TooglePin(&led_Blinky);
}
/*Con esta funcion de interrupcion se cambia el estado de los dos transistores del display y se envia la info al Display*/
void Timer5_Callback(void) {

	gpio_TooglePin(&transistor_sensores);
	gpio_TooglePin(&transistor_resoluciones);
	enviarInfo_al_7_segmentos();
}
/*Con esta funcion de interrupcion se sube la bandera para la actualizacion de datos del ADC y se hace un test de conversion ADC*/
void Timer4_Callback(void) {
	bandera_refresh = SET;
	adc_StartSingleConv();
}
/*Funcion de interrupcion provocada por presionar el boton SW del encoder*/
void callback_extInt5(void) {
	bandera_modo ^= 1;
	/*Dependiendo de la bandera se muestra el mensaje correspondiente en el Cool term*/
	if (bandera_modo == SET ) {
		usart_writeMsg(&usart1, "¡Cambiando a elegir Sensores! *****************************\n\r");
	}
	if (bandera_modo == RESET ) {
		usart_writeMsg(&usart1, "¡Cambiando a elegir Resoluciones! *************************\n\r");
	}
}
/*Funcion de interrupcion provocada por el giro del encoder*/
void callback_extInt3(void) {
	bandera_giro = SET;
}


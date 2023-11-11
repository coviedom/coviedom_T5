
#include <stdint.h>
#include <stm32f4xx.h>
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "stm32_assert.h"
#include "exti_driver_hal.h"


void initSystem(void);
void funcionparacambiarelcanal(ADC_Config_t *a, uint16_t b);
void funcionparacambiarresolucion(ADC_Config_t *c, uint16_t r);
void escribirEnSensores(uint8_t number);
void escribirEnResoluciones(uint8_t number);
void infoDisplay(void);

GPIO_Handler_t _LedBlinky = { 0 };  //pb10
GPIO_Handler_t puntico = { 0 };     //pc6
GPIO_Handler_t ledA = { 0 };   		//pc12
GPIO_Handler_t ledB = { 0 };   		//pc11
GPIO_Handler_t ledC = { 0 };   		//pc5
GPIO_Handler_t ledD = { 0 };   		//pc9
GPIO_Handler_t ledE = { 0 };   		//pc8
GPIO_Handler_t ledG = { 0 };   		//pb9
GPIO_Handler_t _SetSensores = { 0 };      //pb6
GPIO_Handler_t _SetResoluciones = { 0 };  //pb8
Timer_Handler_t _BlinkyTimer3 = { 0 };
Timer_Handler_t _DisplayTimer5 = { 0 };
Timer_Handler_t _Timer4 = { 0 };
EXTI_Config_t _ExtiClock = { 0 }; // Exti 3
GPIO_Handler_t _ClockEncoder = { 0 };	//pc3
GPIO_Handler_t _Data = { 0 }; //pc1
EXTI_Config_t _ExtiModo = { 0 };  //Exti 5
GPIO_Handler_t _BotonModo = { 0 };	//b5
char bufferMsg[128] = { 0 };
GPIO_Handler_t _Tx6 = { 0 };
GPIO_Handler_t _Rx6 = { 0 };
USART_Handler_t Usart6 = { 0 };
ADC_Config_t _Sensor = { 0 };
int8_t contadorSensor = 1;
int8_t contadorResolu = 0;
uint8_t flagModo = 0;
uint8_t flagFlanco = 0;
uint8_t adcComplete = 0;
uint8_t receivedChar = 0;

int main(void) {
	initSystem();

	while (1) {
		if (adcComplete) {
			adcComplete = 0;
			sprintf(bufferMsg, "dato ADC %d,canal %d,resolucion %d\n\r",_Sensor.adcData,contadorSensor,contadorResolu+1);
			usart_writeMsg(&Usart6, bufferMsg);
		}
		if (receivedChar != '\0') {
			if (receivedChar == 'p') {
				usart_writeMsg(&Usart6, "Testing\n\r");
				receivedChar = 0;
			}
			if (receivedChar == 'm') {
				flagModo ^= 1;
				receivedChar = 0;
			}
			if (receivedChar == 'a') {
				if (flagModo == SET) {
					contadorSensor++;
					if (contadorSensor > 3) {
						contadorSensor = 3;
					}
					if (contadorSensor == 1) {
						funcionparacambiarelcanal(&_Sensor, CHANNEL_7);
					}
					if (contadorSensor == 2) {
						funcionparacambiarelcanal(&_Sensor, CHANNEL_6);
					}
					if (contadorSensor == 3) {
						funcionparacambiarelcanal(&_Sensor, CHANNEL_5);
					}
					receivedChar = 0;
				} else if (flagModo == RESET) {
					contadorResolu++;
					if (contadorResolu > 3) {
						contadorResolu = 3;
					}
					if (contadorResolu == 0) {
						funcionparacambiarresolucion(&_Sensor,RESOLUTION_6_BIT);
					}
					if (contadorResolu == 1) {
						funcionparacambiarresolucion(&_Sensor,RESOLUTION_8_BIT);
					}
					if (contadorResolu == 2) {
						funcionparacambiarresolucion(&_Sensor,RESOLUTION_10_BIT);
					}
					if (contadorResolu == 3) {
						funcionparacambiarresolucion(&_Sensor,
								RESOLUTION_12_BIT);
					}
					receivedChar = 0;
				}
			}
			if (receivedChar == 'd') {
				if (flagModo == SET) {
					contadorSensor--;
					if (contadorSensor < 1) {
						contadorSensor = 1;
					}
					if (contadorSensor == 1) {
						funcionparacambiarelcanal(&_Sensor, CHANNEL_7);
					}
					if (contadorSensor == 2) {
						funcionparacambiarelcanal(&_Sensor, CHANNEL_6);
					}
					if (contadorSensor == 3) {
						funcionparacambiarelcanal(&_Sensor, CHANNEL_5);
					}
					receivedChar = 0;
				} else if (flagModo == RESET) {
					contadorResolu--;
					receivedChar = 0;
					if (contadorResolu < 0) {
						contadorResolu = 0;
					}
					if (contadorResolu == 0) {
						funcionparacambiarresolucion(&_Sensor,RESOLUTION_6_BIT);
					}
					if (contadorResolu == 1) {
						funcionparacambiarresolucion(&_Sensor,RESOLUTION_8_BIT);
					}
					if (contadorResolu == 2) {
						funcionparacambiarresolucion(&_Sensor,RESOLUTION_10_BIT);
					}
					if (contadorResolu == 3) {
						funcionparacambiarresolucion(&_Sensor,RESOLUTION_12_BIT);
					}
					receivedChar = 0;
				}
			}
		}
		if (flagFlanco == SET) {
			flagFlanco = RESET;
			uint32_t _Lectura = gpio_ReadPin(&_Data);
			if (flagModo == SET && _Lectura == SET) {
				contadorSensor++;
				if (contadorSensor > 3) {
					contadorSensor = 3;
				}
				if (contadorSensor == 1) {
					funcionparacambiarelcanal(&_Sensor, CHANNEL_7);
				}
				if (contadorSensor == 2) {
					funcionparacambiarelcanal(&_Sensor, CHANNEL_6);
				}
				if (contadorSensor == 3) {
					funcionparacambiarelcanal(&_Sensor, CHANNEL_5);
				}
			}
			if (flagModo == SET && _Lectura == RESET) {
				contadorSensor--;
				if (contadorSensor < 1) {
					contadorSensor = 1;
				}
				if (contadorSensor == 1) {
					funcionparacambiarelcanal(&_Sensor, CHANNEL_7);
				}
				if (contadorSensor == 2) {
					funcionparacambiarelcanal(&_Sensor, CHANNEL_6);
				}
				if (contadorSensor == 3) {
					funcionparacambiarelcanal(&_Sensor, CHANNEL_5);
				}

			}
			if (flagModo == RESET && _Lectura == SET) {
				contadorResolu++;
				if (contadorResolu > 3) {
					contadorResolu = 3;
				}
				if (contadorResolu == 0) {
					funcionparacambiarresolucion(&_Sensor, RESOLUTION_6_BIT);
				}
				if (contadorResolu == 1) {
					funcionparacambiarresolucion(&_Sensor, RESOLUTION_8_BIT);
				}
				if (contadorResolu == 2) {
					funcionparacambiarresolucion(&_Sensor, RESOLUTION_10_BIT);
				}
				if (contadorResolu == 3) {
					funcionparacambiarresolucion(&_Sensor, RESOLUTION_12_BIT);
				}
			}
			if (flagModo == RESET && _Lectura == RESET) {
				contadorResolu--;
				if (contadorResolu < 0) {
					contadorResolu = 0;
				}
				if (contadorResolu == 0) {
					funcionparacambiarresolucion(&_Sensor, RESOLUTION_6_BIT);
				}
				if (contadorResolu == 1) {
					funcionparacambiarresolucion(&_Sensor, RESOLUTION_8_BIT);
				}
				if (contadorResolu == 2) {
					funcionparacambiarresolucion(&_Sensor, RESOLUTION_10_BIT);
				}
				if (contadorResolu == 3) {
					funcionparacambiarresolucion(&_Sensor, RESOLUTION_12_BIT);
				}
			}
			infoDisplay();
		}
	}
}

void initSystem(void) {

	puntico.pGPIOx = GPIOC;
	puntico.pinConfig.GPIO_PinNumber = PIN_6;
	puntico.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	puntico.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	puntico.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	puntico.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&puntico);

	ledA.pGPIOx = GPIOC;
	ledA.pinConfig.GPIO_PinNumber = PIN_12;
	ledA.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledA.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledA.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledA.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledA);

	ledB.pGPIOx = GPIOC;
	ledB.pinConfig.GPIO_PinNumber = PIN_11;
	ledB.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledB.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledB.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledB.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledB);

	ledC.pGPIOx = GPIOC;
	ledC.pinConfig.GPIO_PinNumber = PIN_5;
	ledC.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledC.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledC.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledC.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledC);

	ledD.pGPIOx = GPIOC;
	ledD.pinConfig.GPIO_PinNumber = PIN_9;
	ledD.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledD.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledD.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledD.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledD);

	ledE.pGPIOx = GPIOC;
	ledE.pinConfig.GPIO_PinNumber = PIN_8;
	ledE.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledE.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledE.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledE.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledE);

	ledG.pGPIOx = GPIOB;
	ledG.pinConfig.GPIO_PinNumber = PIN_9;
	ledG.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledG.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	ledG.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledG.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledG);

	_LedBlinky.pGPIOx = GPIOB;
	_LedBlinky.pinConfig.GPIO_PinNumber = PIN_10;
	_LedBlinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_LedBlinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_LedBlinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_LedBlinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_LedBlinky);

	_SetSensores.pGPIOx = GPIOB;
	_SetSensores.pinConfig.GPIO_PinNumber = PIN_6;
	_SetSensores.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_SetSensores.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_SetSensores.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_SetSensores.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_SetSensores);

	_SetResoluciones.pGPIOx = GPIOB;
	_SetResoluciones.pinConfig.GPIO_PinNumber = PIN_8;
	_SetResoluciones.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	_SetResoluciones.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_SetResoluciones.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_SetResoluciones.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_SetResoluciones);

	_Data.pGPIOx = GPIOC;
	_Data.pinConfig.GPIO_PinNumber = PIN_1;
	_Data.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	_Data.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Data.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_Data.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&_Data);

	_ClockEncoder.pGPIOx = GPIOC;
	_ClockEncoder.pinConfig.GPIO_PinNumber = PIN_3;
	_ClockEncoder.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	_ClockEncoder.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_ClockEncoder.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_ClockEncoder.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	_ExtiClock.pGPIOHandler = &_ClockEncoder;
	_ExtiClock.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	exti_Config(&_ExtiClock);

	_BotonModo.pGPIOx = GPIOB;
	_BotonModo.pinConfig.GPIO_PinNumber = PIN_5;
	_BotonModo.pinConfig.GPIO_PinMode = GPIO_MODE_IN;
	_BotonModo.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_BotonModo.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	_BotonModo.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	_ExtiModo.pGPIOHandler = &_BotonModo;
	_ExtiModo.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	exti_Config(&_ExtiModo);

	Usart6.ptrUSARTx = USART6;
	Usart6.USART_Config.baudrate = USART_BAUDRATE_115200;
	Usart6.USART_Config.datasize = USART_DATASIZE_8BIT;
	Usart6.USART_Config.parity = USART_PARITY_NONE;
	Usart6.USART_Config.stopbits = USART_STOPBIT_1;
	Usart6.USART_Config.mode = USART_MODE_RXTX;
	Usart6.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;

	usart_Config(&Usart6);

	_Tx6.pGPIOx = GPIOA;
	_Tx6.pinConfig.GPIO_PinNumber = PIN_11;
	_Tx6.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_Tx6.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Tx6.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	_Tx6.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_Tx6.pinConfig.GPIO_PinAltFunMode = AF8;

	gpio_Config(&_Tx6);

	_Rx6.pGPIOx = GPIOA;
	_Rx6.pinConfig.GPIO_PinNumber = PIN_12;
	_Rx6.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	_Rx6.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	_Rx6.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	_Rx6.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	_Rx6.pinConfig.GPIO_PinAltFunMode = AF8;

	gpio_Config(&_Rx6);

	_Sensor.channel = CHANNEL_7;
	_Sensor.resolution = RESOLUTION_6_BIT;
	_Sensor.dataAlignment = ALIGNMENT_RIGHT;
	_Sensor.samplingPeriod = SAMPLING_PERIOD_84_CYCLES;
	_Sensor.interrupState = ADC_INT_ENABLE;
	adc_ConfigSingleChannel(&_Sensor);

	_BlinkyTimer3.pTIMx = TIM3;
	_BlinkyTimer3.TIMx_Config.TIMx_Prescaler = 16000;
	_BlinkyTimer3.TIMx_Config.TIMx_Period = 250;
	_BlinkyTimer3.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	_BlinkyTimer3.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	timer_Config(&_BlinkyTimer3);
	timer_SetState(&_BlinkyTimer3, TIMER_ON);

	_DisplayTimer5.pTIMx = TIM5;
	_DisplayTimer5.TIMx_Config.TIMx_Prescaler = 16000;
	_DisplayTimer5.TIMx_Config.TIMx_Period = 10;
	_DisplayTimer5.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	_DisplayTimer5.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	timer_Config(&_DisplayTimer5);
	timer_SetState(&_DisplayTimer5, TIMER_ON);

	_Timer4.pTIMx = TIM4;
	_Timer4.TIMx_Config.TIMx_Prescaler = 16000;
	_Timer4.TIMx_Config.TIMx_Period = 5000;
	_Timer4.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	_Timer4.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;
	timer_Config(&_Timer4);
	timer_SetState(&_Timer4, TIMER_ON);
	gpio_WritePin(&_SetSensores, SET);
	gpio_WritePin(&_SetResoluciones, RESET);
}
void funcionparacambiarelcanal(ADC_Config_t *a, uint16_t b) {
	a->channel = b;
	adc_ConfigSingleChannel(a);
}
void funcionparacambiarresolucion(ADC_Config_t *c, uint16_t r) {
	c->resolution = r;
	adc_ConfigSingleChannel(c);
}
void adc_CompleteCallback(void) {
	adcComplete = 1;
	_Sensor.adcData = adc_GetValue();
}
void usart6_RxCallback(void) {
	receivedChar = usart_getRxData();
}
void Timer3_Callback(void) {
	gpio_TooglePin(&_LedBlinky);
}
void Timer5_Callback(void) {
	gpio_TooglePin(&_SetSensores);
	gpio_TooglePin(&_SetResoluciones);
	infoDisplay();
}
void Timer4_Callback(void) {
	adc_StartSingleConv();
}
void callback_extInt5(void) {
	flagModo ^= 1;
}
void callback_extInt3(void) {
	flagFlanco = SET;
}
void infoDisplay(void) {
	uint32_t left = gpio_ReadPin(&_SetSensores);
	if (left == SET) {
		escribirEnResoluciones(contadorResolu);
		if (flagModo == SET) {
			gpio_WritePin(&puntico, SET);
		}
	}
	uint32_t right = gpio_ReadPin(&_SetResoluciones);
	if (right == SET) {
		escribirEnSensores(contadorSensor);
		if (flagModo == RESET) {
			gpio_WritePin(&puntico, SET);
		}
	}
}
void escribirEnResoluciones(uint8_t resolucion) {
	switch (resolucion) {
	case 0:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&ledA, SET);
		gpio_WritePin(&ledB, SET);
		gpio_WritePin(&ledC, SET);
		gpio_WritePin(&ledD, SET);
		gpio_WritePin(&ledE, RESET);
		gpio_WritePin(&ledG, SET);
		break;

	case 1:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&ledA, SET);
		gpio_WritePin(&ledB, SET);
		gpio_WritePin(&ledC, SET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledG, SET);
		break;
	case 2:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&ledA, SET);
		gpio_WritePin(&ledB, SET);
		gpio_WritePin(&ledC, SET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledG, RESET);
		break;
	case 3:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, SET);
		gpio_WritePin(&ledC, SET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledG, RESET);
		break;
	default: {
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, SET);
		gpio_WritePin(&ledC, SET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledG, RESET);
		break;
	}
	}
}
void escribirEnSensores(uint8_t sensor) {
	switch (sensor) {
	case 1:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&ledA, SET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, SET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledG, SET);
		break;
	case 2:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, SET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, RESET);
		gpio_WritePin(&ledG, RESET);
		break;
	case 3:
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&ledA, RESET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, RESET);
		gpio_WritePin(&ledE, SET);
		gpio_WritePin(&ledG, RESET);
		break;
	default: {
		gpio_WritePin(&puntico, RESET);
		gpio_WritePin(&ledA, SET);
		gpio_WritePin(&ledB, RESET);
		gpio_WritePin(&ledC, RESET);
		gpio_WritePin(&ledD, SET);
		gpio_WritePin(&ledE, SET);
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

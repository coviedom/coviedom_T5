#include <stdint.h>
#include <stm32f4xx.h>
#include <stdbool.h>
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "stm32_assert.h"
#include "exti_driver_hal.h"
#include "systick_driver_hal.h"
#include "pwm_driver_hal.h"
#include "i2c_driver_hal.h"

void initSys(void);

Timer_Handler_t Timer5Blinky = { 0 };

GPIO_Handler_t Blinky = { 0 };

Timer_Handler_t verificacion = { 0 };

USART_Handler_t Usart2 = { 0 };
GPIO_Handler_t Usart2TA2 = { 0 };  //PIN A2 tranmicion
GPIO_Handler_t Usart2RA3 = { 0 };  //PIN A3 recepcion

char bufferData[128] = "accel MPU-6050 testing...";

uint32_t systemTicks = 0;
uint32_t systemTicksStart = 0;
uint32_t systemTicksEnd = 0;

I2C_Handler_t Acelerometro = { 0 };
GPIO_Handler_t I2cSDA = { 0 };  //PIN A2 tranmicion
GPIO_Handler_t I2cSCL = { 0 };  //PIN A3 recepcion

uint8_t rxData = 0;
uint8_t i2cBuffer = 0;

#define ACCEL_ADDRESS   0b1101001
#define ACCEL_XOUT_H    59
#define ACCEL_XOUT_L    60
#define ACCEL_YOUT_H    61
#define ACCEL_YOUT_L    62
#define ACCEL_ZOUT_H    63
#define ACCEL_ZOUT_L    64

#define PWR_MGMT_1  107
#define WHO_AM_I    117

int main() {
	initSys();
	while (1) {
		if (rxData != '\0') {
			usart_WriteChar(&Usart2, rxData);
			if (rxData == 'w') {
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				usart_writeMsg(&Usart2, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Acelerometro, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n",
						(unsigned int) i2cBuffer);
				usart_writeMsg(&Usart2, bufferData);

				rxData = '\0';

			} else if (rxData == 'p') {
				sprintf(bufferData, "PWR_MGMT_1 (r)\n");
				usart_writeMsg(&Usart2, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Acelerometro, PWR_MGMT_1);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				usart_writeMsg(&Usart2, bufferData);

				rxData = '\0';
			} else if (rxData == 'r') {
				sprintf(bufferData, "PWR_MGMT_1 reset (r)\n");
				usart_writeMsg(&Usart2, bufferData);

				i2c_writeSingleRegister(&Acelerometro, PWR_MGMT_1, 0x00);

				rxData = '\0';
			} else if (rxData != 'x') {
				sprintf(bufferData, "Axis X data (r)\n");
				usart_writeMsg(&Usart2, bufferData);

				uint8_t AccelX_low = i2c_readSingleRegister(&Acelerometro, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&Acelerometro, ACCEL_XOUT_H);
				uint16_t AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData, "AccelX = %d (r)\n", (int) AccelX);
				usart_writeMsg(&Usart2, bufferData);

				rxData = '\0';
			} else if (rxData != 'y') {
				sprintf(bufferData, "Axis Y data (r)\n");
				usart_writeMsg(&Usart2, bufferData);

				uint8_t AccelY_low = i2c_readSingleRegister(&Acelerometro, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&Acelerometro, ACCEL_YOUT_H);
				uint16_t AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %d (r)\n", (int) AccelY);
				usart_writeMsg(&Usart2, bufferData);

				rxData = '\0';
			} else if (rxData != 'z') {
				sprintf(bufferData, "Axis Z data (r)\n");
				usart_writeMsg(&Usart2, bufferData);

				uint8_t AccelZ_low = i2c_readSingleRegister(&Acelerometro, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&Acelerometro, ACCEL_ZOUT_H);
				uint16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %d (r)\n", (int) AccelZ);
				usart_writeMsg(&Usart2, bufferData);

				rxData = '\0';
			} else {
				rxData = '\0';
			}
		}
	}
}
void initSys(void) {
	/* Configuramos el pin del blinky */
	Blinky.pGPIOx = GPIOB;
	Blinky.pinConfig.GPIO_PinNumber = PIN_10;
	Blinky.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	Blinky.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Blinky.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	Blinky.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	gpio_Config(&Blinky);

	//Configuracion ledA
	I2cSCL.pGPIOx = GPIOB;
	I2cSCL.pinConfig.GPIO_PinNumber = PIN_8;
	I2cSCL.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2cSCL.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_OPENDRAIN;
	I2cSCL.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	I2cSCL.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;

	gpio_Config(&I2cSCL);

	I2cSDA.pGPIOx = GPIOB;
	I2cSDA.pinConfig.GPIO_PinNumber = PIN_9;
	I2cSDA.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2cSDA.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_OPENDRAIN;
	I2cSDA.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	I2cSDA.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;

	gpio_Config(&I2cSDA);

	Usart2TA2.pGPIOx = GPIOA;
	Usart2TA2.pinConfig.GPIO_PinNumber = PIN_2;
	Usart2TA2.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Usart2TA2.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Usart2TA2.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	Usart2TA2.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Usart2TA2.pinConfig.GPIO_PinAltFunMode = AF7;

	gpio_Config(&Usart2TA2);

	/* Configuramos el PinA3 para el USART2*/
	Usart2RA3.pGPIOx = GPIOA;
	Usart2RA3.pinConfig.GPIO_PinNumber = PIN_3;
	Usart2RA3.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Usart2RA3.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	Usart2RA3.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_HIGH;
	Usart2RA3.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	Usart2RA3.pinConfig.GPIO_PinAltFunMode = AF7;

	gpio_Config(&Usart2RA3);
	/* Configuramos el USART2*/
	Usart2.ptrUSARTx = USART2;
	Usart2.USART_Config.datasize = USART_DATASIZE_8BIT;
	Usart2.USART_Config.mode = USART_MODE_RXTX;
	Usart2.USART_Config.parity = USART_PARITY_NONE;
	Usart2.USART_Config.stopbits = USART_STOPBIT_1;
	Usart2.USART_Config.enableIntTX = USART_TX_INTERRUP_DISABLE;
	Usart2.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	Usart2.USART_Config.baudrate = USART_BAUDRATE_115200;

	usart_Config(&Usart2);

	//Configuracion del TIM5
	Timer5Blinky.pTIMx = TIM5;
	Timer5Blinky.TIMx_Config.TIMx_Prescaler = 16000; // Genera incrementos de 1 ms
	Timer5Blinky.TIMx_Config.TIMx_Period = 400; // De la mano con el prescaler
	Timer5Blinky.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	Timer5Blinky.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&Timer5Blinky);

	timer_SetState(&Timer5Blinky, TIMER_ON);

}

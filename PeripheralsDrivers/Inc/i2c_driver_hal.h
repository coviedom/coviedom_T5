/*
 * i2c_driver_hal.h
 *
 *  Created on: 11/11/2023
 *      Author: Cristhian Oviedo
 */

#ifndef I2C_DRIVER_HAL_H_
#define I2C_DRIVER_HAL_H_

#include <stm32f4xx.h>

enum
{
	I2C_WRITE_DATA = 0,
	I2C_READ_DATA
};

#define MAIN_CLOCK_4_MHz_FOR_I2C		4
#define MAIN_CLOCK_16_MHz_FOR_I2C		16
#define MAIN_CLOCK_20_MHz_FOR_I2C		20

enum
{
	I2C_MODE_SM	 = 0,
	I2C_MODE_FM
};

#define I2C_MODE_SM_SPEED_100KHz		80
#define I2C_MODE_FM_SPEED_400KHz		1

#define I2C_MAX_RISE_TIME_SM			17
#define I2C_MAX_RISE_TIME_FM			5

typedef struct
{
	I2C_TypeDef		*ptrI2Cx;
	uint8_t			slaveAddress;
	uint8_t			modeI2C;
	uint8_t			dataI2C;
}I2C_Handler_t;

/*Prototipos de las funciones publicas*/
void i2c_Config(I2C_Handler_t *ptrHandlerI2C);
uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead);
void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue);


#endif /* I2C_DRIVER_HAL_H_ */

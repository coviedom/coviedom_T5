#include <stdint.h>
#include "gpio_driver_hal.h"
#include <stm32f4xx.h>
#include "stm32_assert.h"

//Definimos un pin de prueba
GPIO_Handler_t userLed = {0};

int main(void)
{

	/* Configuramos el pin */

	userLed.pGPIOx                 = GPIOA;
	userLed.pinConfig.GPIO_PinNumber = PIN_5;
	userLed.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	/* Cargamos la configuracion en los registros que gobiernan el puerto */

	gpio_Config(&userLed);
	gpio_WritePin(&userLed, SET);

	while (1) {
		gpio_TooglePin(&userLed);
		for (uint32_t j = 0; j < 200000; j++) {
			__NOP();
		}
	}
}
/*Esta funcion sirve para detectar problemas de parametros*/
void assert_failed(uint8_t* file, uint32_t line) {

	while(1) {
// problems...
	}
}

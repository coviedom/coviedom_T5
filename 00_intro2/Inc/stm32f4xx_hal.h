/*
 * stm32f4xx_hal.h
 *
 *  Created on: SEptiembre 7
 *      Author: Cristhian Oviedo
 *
 *Este archivo contiene la informacion mas basica del micro:
 *- Valores del reloj principal
 *- Distribucion basica de la memoria (descrito en la figura 14 de la hoja de datos del micro)
 *- Posiciones de memoria de los perifericos disponibles en el micro descrito en la tabla 1 (Memory Map)
 *- Incluir los demas drivers de los perifericos
 *- definiciones de las demas contantes mas basicas.
 *
 *NOTA: La definicion del NVIC sera realizada al momento de describir el uso de las interrupciones
 */


#ifndef STM32F4XX_HAL_H_
#define STM32F4XX_HAL_H_
#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK SPEED     8000000    //Value for the main clock signal (HSI-> High speed Internal)
#define HSE_CLOCK SPEED     16000000   //Value for the main clock signal (HSE-> High speed External)

//#define NOP()      (    asm("NOP"))
#define NOP()        asm("NOP")
#define _weak        _attribute_((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, Memory Map, Figure 14
 * (Remember, 1KByte = 1024 bytes
 */
#define FLASH_BASE_ADDR     0x08000000U  // Esta es la memoria del programa, 512KB
#define SRAM_BASE_ADDR      0x20000000U  // Esta es la memoria RAM, 128KB.

/* NOTA: Observar que existen unos registros especificos del Cortex M4 en la region 0xE0000000U
 * los controladores de las interrupciones se encuentran alli, por ejemplo. Esto se vara a su debido
 * tiempo
 */

/*
 * NOTA:
 * Ahora agregamos la direccion de memoria base para cada uno de los perifericos que posee el micro
 * En el "datasheet" del icro, figura 14 (Memory Map) encontramos el mapa de los buses:
 *    -APB1 (Advance Peripheral Bus)
 *    -APB2
 *    -AHB1 (Advance High-performance Bus)
 *    -AHB2
 */

 /**
  * AHBx and APBx Bus peripherals bases addresses
  */
#define APB1_BASE_ADDR     0X40000000U
#define APB2_BASE_ADDR     0X40010000U
#define AHB1_BASE_ADDR     0X40020000U
#define AHB2_BASE_ADDR     0X50000000U

/**
 * Y ahora debemos hacer lo mismo pero cada una de las posiciones de memoria de cada uno de los perifericos
 * descritos en la Tabla 1 de manual de referencia del micro.
 * Observe que en dicha tala esta a su vez dividida en cuatro segmentos. cada uno correspondiente a
 * APB1, APB2, AHB1, AHB2.
 *
 * Comenzar de arriba hacia abajo como se muestra en la tabla. Inicia USB_OTG_F5 (AHB2)
 */

/* Posiciones de memoria para perifericos del AHB2 */
#define USB_OTG_F5_BASE_ADDR    (AHB2_BASE_ADDR + 0x0000U)

/* Posiciones de memoria para perifericos del AHB1 */
#define DMA2_BASE_ADDR     (AHB1_BASE_ADDR + 0x6400U)
#define DMA1_BASE_ADDR     (AHB1_BASE_ADDR + 0x6000U)
#define FIR_BASE_ADDR      (AHB1_BASE_ADDR + 0x3C00U)   // Flash Interface Register...
#define RCC_BASE_ADDR      (AHB1_BASE_ADDR + 0x3800U)
#define CRC_BASE_ADDR      (AHB1_BASE_ADDR + 0x3000U)
#define GPIOH_BASE_ADDR    (AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR    (AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR    (AHB1_BASE_ADDR + 0x0C00U)}
#define GPIOC_BASE_ADDR    (AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR    (AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR    (AHB1_BASE_ADDR + 0x0000U)

/* Posiciones de memoria para perifericos del APB2*/
//#define SPI5_BASE_ADDR      (APB2_BASE_ADDR + 0x5000U)
//#define TIM11_BASE_ADDR     (APB2_BASE_ADDR + 0x4800U)
//#define TIM10_BASE_ADDR     (APB2_BASE_ADDR + 0x4400U)
//#define TIM9_BASE_ADDR      (APB2_BASE_ADDR + 0x4000U)
//#define EXTI_BASE_ADDR      (APB2_BASE_ADDR + 0x3C00U)
//#define SYSCFG_BASE_ADDR    (APB2_BASE_ADDR + 0x3800U)
//#define SP14_BASE_ADDR      (APB2_BASE_ADDR + 0x3400U)
//#define SPI1_BASE_ADDR      (APB2_BASE_ADDR + 0x3000U)
//#define SDIO_BASE_ADDR      (APB2_BASE_ADDR + 0x2C00U)
//#define ADC1_BASE_ADDR      (APB2_BASE_ADDR + 0x2000U)
//define  USART6_BASE_ADDR    (APB2_BASE_ADDR + 0x1400U)
//define  USART1_BASE_ADDR    (APB2_BASE_ADDR + 0x1000U)
//define  TIM1_BASE_ADDR      (APB2_BASE_ADDR + 0x0000U)

/* Posiciones de memoria para perifericos del APB1 */
//#define PWR_BASE_ADDR       (APB1_BASE_ADDR + 0X7000U)
//#define I2C3_BASE_ADDR      (APB1_BASE_ADDR + 0X5C00U)
//#define I2C2_BASE_ADDR      (APB1_BASE_ADDR + 0X5800U)
//#define I2C1_BASE_ADDR      (APB1_BASE_ADDR + 0X5400U)
//#define USART2_BASE_ADDR    (APB1_BASE_ADDR + 0X4400U)
//#define I2Sext_BASE_ADDR    (APB1_BASE_ADDR + 0X4000U)
//#define SPI3_BASE_ADDR      (APB1_BASE_ADDR + 0X3C00U)
//#define SPI2_BASE_ADDR      (APB1_BASE_ADDR + 0X3800U)
//#define I2S2_BASE_ADDR      (APB1_BASE_ADDR + 0X3400U)
//#define IWDG_BASE_ADDR      (APB1_BASE_ADDR + 0X3000U)
//define  WWDG_BASE_ADDR      (APB1_BASE_ADDR + 0X2C00U)
//define  RTC_BASE_ADDR       (APB1_BASE_ADDR + 0X2800U)
//define  TIM5_BASE_ADDR      (APB1_BASE_ADDR + 0X0C00U)
//define  TIM4_BASE_ADDR      (APB1_BASE_ADDR + 0X0800U)
//define  TIM3_BASE_ADDR      (APB1_BASE_ADDR + 0X0400U)
//define  TIM2_BASE_ADDR      (APB1_BASE_ADDR + 0X0000U)

/**
 *  Macros Genericos
 **/
#define  DISABLE        (0)
#define  ENABLE         (1)
#define  SET            ENABLE
#define  CLEAR          DISABLE
#define  RESET          DISABLE
#define  FLAG_SET       SET
#define  FLAG_RESET     RESET
#define  I2C_WRITE      (0)
#define  I2C_READ       (1)

/* +++======  INICIO  de la descripcion de los elementos que componen el periferico ========+++ */

/* definicion de la estructura de datos  que representan a cada uno de los  registros que componen el
 * periferico RCC.
 *
 * Debido a los temas que s evan a manejar en el curso solo deben defiir los bits de los registros:
 * 6.3.1 (RCC_R)  hasta el 6.3.12 (RCC_APB2ENR), 6.3.17 (RCC_BDCR) Y 6.3.18 (RCC_CSR)
 *
 * Nota: la posicion de memoria (offset) debe encajar perfectamente con la posicion de memoria indicada
 * en la hoj de datos del equipo. Observa que los elementos "reserbadps" tambien estan presentes alli.
 */
typedef struct
{
	volatile uint32_t CR;         // Clock control register                    ADDR_OFFSET   0X0
	volatile uint32_t PLLCFGR;    // PLL Configuration register                ADDR_OFFSET   0x04
	volatile uint32_t CFGR;       // Clock Configuratio register               ADDR_OFFSET   0x08
	volatile uint32_t CIR;        // Clock Interruptor register                ADDR_OFFSET   0x0c
	volatile uint32_t AHB1RSTR;   // AHB1 Perispheral reset register           ADDR_OFFSET   0x10
	volatile uint32_t AHB2RSTR;   // AHB2 periphersl reset register            ADDR_OFFSET   0x14
	volatile uint32_t reserved0;  // reserved                                  ADDR_OFFSET   0X18
	volatile uint32_t reserved1;  // reserved                                  ADDR_OFFSET   0X1C
	volatile uint32_t APB1RSTR;   // APB1 periphersl reset register            ADDR_OFFSET   0x20
	volatile uint32_t APB2RSTR;   // APB2 periphersl reset register            ADDR_OFFSET   0x24
	volatile uint32_t reserved2;  // reserved                                  ADDR_OFFSET   0X28
	volatile uint32_t reserved3;  // reserved                                  ADDR_OFFSET   0X2C
	volatile uint32_t AHB1ENR;    // AHB1 peripheral clock enable register     ADDR_OFFSET   0x30
	volatile uint32_t AHB2ENR;    // AHB2 peripheral clock enable register     ADDR_OFFSET   0x34
	volatile uint32_t reserved4;  // reserved                                  ADDR_OFFSET   0X38
	volatile uint32_t reserved5;  // reserved                                  ADDR_OFFSET   0X3c
	volatile uint32_t APB1ENR;    // APB1 peripheral clock enable register     ADDR_OFFSET   0x40
    volatile uint32_t APB2ENR;    // APB2 peripheral clock enable register     ADDR_OFFSET   0x44
    volatile uint32_t reserved6;  // reserved                                  ADDR_OFFSET   0X48
    volatile uint32_t reserved7;  // reserved                                  ADDR_OFFSET   0X4C
	volatile uint32_t AHB1LPENR;  // AHB1 clock enable low power register      ADDR_OFFSET   0x50
	volatile uint32_t AHB2LPENR;  // AHB2 clock enable low power register      ADDR_OFFSET   0x54
	volatile uint32_t reserved8;  // reserved                                  ADDR_OFFSET   0X58
    volatile uint32_t reserved9;  // reserved                                  ADDR_OFFSET   0X5C
    volatile uint32_t APB1LPENR;  // APB1 clock enable low power register      ADDR_OFFSET   0x60
    volatile uint32_t APB2LPENR;  // APB2 clock enable low power register      ADDR_OFFSET   0x64
	volatile uint32_t reserved10; // reserved                                  ADDR_OFFSET   0X68
    volatile uint32_t reserved11; // reserved                                  ADDR_OFFSET   0X6C
	volatile uint32_t BDCR;       // Backup domain control register            ADDR_OFFSET   0x70
	volatile uint32_t CSR;        // Clock control & status register           ADDR_OFFSET   0x74
	volatile uint32_t reserved12; // reserved                                  ADDR_OFFSET   0X78
    volatile uint32_t reserved13; // reserved                                  ADDR_OFFSET   0X7C
	volatile uint32_t SSCGR;      // Spread spectrum clock generation reg       ADDR_OFFSET  0x80
	volatile uint32_t PLLI2SCFGR; // PLLI2S Configuration register             ADDR_OFFSET   0X84
    volatile uint32_t reserved14; // reserved                                  ADDR_OFFSET   0X88
	volatile uint32_t DCKCFGR;    // Dedicated Clock Configuration reg         ADDR_OFFSET   0X8C
} RCC_RegDef_t;

/*
 * Hacemos como un "merge", en el cual ubicamos la estructura RCC_RegDef_t a apuntar a la posicion
 * de memoria correspondiente, de forma que cada variable dentro de la estructura coincide con cada
 * uno de los SFR en la memoria del micro-controlador
 * */
#define RCC             ((RCC_RegDef_t *) RCC_BASE_ADDR)


/* Descripcion bit a bit de cada uno de los registros del que componen al periferico RCC */
/* 6.3.1 RCC_CR */
/* 6.3.2 RCC_PLLCFGR */
/* 6.3.3 RCC_CFGR */
/* 6.3.4 RCC_CIR */
/* 6.3.5 RCC_AHB1RSTR */
/* 6.3.6 RCC_AHB2RSTR */
/* 6.3.7 RCC_APB1RSTR */
/* 6.3.8 RCC_APB2RSTR */

/* 6.3.9 RCC_AHB1ENR */
#define RCC_AHB1ENR_GPIOAEN         0
#define RCC_AHB1ENR_GPIOBEN         1
#define RCC_AHB1ENR_GPIOCEN         2
#define RCC_AHB1ENR_GPIODEN         3
#define RCC_AHB1ENR_GPIOEEN         4
#define RCC_AHB1ENR_GPIOHEN         7
#define RCC_AHB1ENR_CRCEN           12
#define RCC_AHB1ENR_DMA1EN          21
#define RCC_AHB1ENR_DMA2EN          22

/* 6.3.10 RCC_AHB2ENR */
/* 6.3.11 RCC_APB1ENR */
/* 6.3.12 RCC_APB2ENR */
/* 6.3.17 RCC_BDCR */
/* 6.3.18 RCC_CSR */


/* ======= FIN de la descripcion de los elementos que componen el periférico ======= */

/* ++++++++ posibles valores de las configuraciones */
/* System Clock (main clock) possible options */
/* Microcontroller Clock Output 1 (MC01) options */
/* Microcontroller preescaler Clock Output1 (MCO1PRE) options */
/* Microcontroller preescaler Clock Output1 (MCO1PRE) options */
/* Microcontroller Clock Output 2 (MC02) options */

/* +++======= INICIO de la descripcion de los elementos que componen el periférico =======+++ */
/* Definicion de la estructura de datos que representa a cada uno de los registros que componen
 * el periferico GPIO.
 *
 * Debido a que el periferico GPIOx es muy simple, no es muy necesario crear la descripcion bit a bit
 * de cada uno de los registros que componen el dicho periférico, pero si es necesario comprender que
 * hace cada registro, para poder cargar correctamente la configuracion.
 *
 */

typedef struct
{
    volatile uint32_t MODER;    // port mode register                  ADDR_OFFSET:     0x00
    volatile uint32_t OTYPER;   // port output type register           ADDR_OFFSET:     0x04
    volatile uint32_t OSPEEDR;  // port output speed register          ADDR_OFFSET:     0x08
    volatile uint32_t PUPDR;    // port pull-up/pull-down register     ADDR_OFFSET:     0x0C
    volatile uint32_t IDR;      // port input data register            ADDR_OFFSET:     0x10
    volatile uint32_t ODR;      // port output data register           ADDR_OFFSET:     0x14
    volatile uint32_t BSRR;     // port bit set/reset register         ADDR_OFFSET:     0x18
    volatile uint32_t LCKR;     // port configuration lock register    ADDR_OFFSET:     0x1C
    volatile uint32_t AFR[2];   // port alternate function registers,  ADDR_OFFSET:     0x20-0x24
} GPIO_TypeDef;

/* Creamos un objeto de la estructura definida y hacemos que quede ubicada exactamente sobre
 * la posición de memoria donde se encuentra en registro
 * Debido a que son varios periféricos GPIOx, es necesario hacer la definición para cada uno.
 *
 * Tener cuidado que cada elemento coincida con su respectiva dirección base.
 * */
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE_ADDR)
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE_ADDR)
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE_ADDR)
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE_ADDR)
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE_ADDR)
#define GPIOH ((GPIO_TypeDef *) GPIOH_BASE_ADDR)

/* For testing assert parameters.*/
#define IS_GPIO_ALL_INSTANCE(GPIOx) (GPIOx == GPIOA) || \
									(GPIOx == GPIOB) || \
									(GPIOx == GPIOC) || \
								    (GPIOx == GPIOD) || \
									(GPIOx == GPIOE) || \
									(GPIOx == GPIOH)


/*
typedef struct
{
	volatile uint32_t dummy;	// Dummy Example register	ADDR_OFFSET:	0x00
} DUMMY_RegDef_t;
*/

/* No es necesario hacer nada mas en este archivo, para este primer proyecto */

/* Incluimos las librerías de cada periférico*/

#endif /* _STM32F4XX_HAL_H_ */

#ifndef __GPIO_H
#define __GPIO_H

// Includes --------------------------------------------------------------------
// #include "main.h"
#include "stm32f1_gpio.h"

// Typedef ---------------------------------------------------------------------

// Macro -----------------------------------------------------------------------

// GPIOC Pin12 - LED
#define LED_PORT GPIOC
#define LED_PIN  GPIO_PIN_12

#define FIRE_MON_CUR_Pin       GPIO_PIN_0
#define FIRE_MON_CUR_GPIO_Port GPIOA
#define HEAT_MON_V_Pin         GPIO_PIN_1
#define HEAT_MON_V_GPIO_Port   GPIOA
#define HEAT_MON_CUR_Pin       GPIO_PIN_2
#define HEAT_MON_CUR_GPIO_Port GPIOA
#define FIRE_MON_V_Pin         GPIO_PIN_3
#define FIRE_MON_V_GPIO_Port   GPIOA
#define PA4_Pin                GPIO_PIN_4
#define PA4_GPIO_Port          GPIOA
#define PA5_Pin                GPIO_PIN_5
#define PA5_GPIO_Port          GPIOA
#define PA6_Pin                GPIO_PIN_6
#define PA6_GPIO_Port          GPIOA
#define PA7_Pin                GPIO_PIN_7
#define PA7_GPIO_Port          GPIOA
#define PB0_Pin                GPIO_PIN_0
#define PB0_GPIO_Port          GPIOB
#define FIRE_FAULT_Pin         GPIO_PIN_12
#define FIRE_FAULT_GPIO_Port   GPIOB
#define FIRE_DONE_Pin          GPIO_PIN_13
#define FIRE_DONE_GPIO_Port    GPIOB
#define FIRE_CHARGE_Pin        GPIO_PIN_14
#define FIRE_CHARGE_GPIO_Port  GPIOB
#define PC7_Pin                GPIO_PIN_7
#define PC7_GPIO_Port          GPIOC
#define PC8_Pin                GPIO_PIN_8
#define PC8_GPIO_Port          GPIOC
#define PC9_Pin                GPIO_PIN_9
#define PC9_GPIO_Port          GPIOC
#define TX_Pin                 GPIO_PIN_9
#define TX_GPIO_Port           GPIOA
#define RX_Pin                 GPIO_PIN_10
#define RX_GPIO_Port           GPIOA
#define SWDIO_Pin              GPIO_PIN_13
#define SWDIO_GPIO_Port        GPIOA
#define SWCLK_Pin              GPIO_PIN_14
#define SWCLK_GPIO_Port        GPIOA
#define PA15_Pin               GPIO_PIN_15
#define PA15_GPIO_Port         GPIOA
#define PC10_Pin               GPIO_PIN_10
#define PC10_GPIO_Port         GPIOC
#define HEAT_CS_Pin            GPIO_PIN_12
#define HEAT_CS_GPIO_Port      GPIOC
#define SPI_SCLK_Pin           GPIO_PIN_3
#define SPI_SCLK_GPIO_Port     GPIOB
#define SPI_MISO_Pin           GPIO_PIN_4
#define SPI_MISO_GPIO_Port     GPIOB
#define SPI_MOSI_Pin           GPIO_PIN_5
#define SPI_MOSI_GPIO_Port     GPIOB
#define FIRE_CS_Pin            GPIO_PIN_6
#define FIRE_CS_GPIO_Port      GPIOB
#define HEAT_FAULT_Pin         GPIO_PIN_7
#define HEAT_FAULT_GPIO_Port   GPIOB
#define HEAT_EN_Pin            GPIO_PIN_9
#define HEAT_EN_GPIO_Port      GPIOB
// Variables -------------------------------------------------------------------

// Function prototypes ---------------------------------------------------------
void GpioInit(void);

// Inline Functions ------------------------------------------------------------
__STATIC_FORCEINLINE void LedOn(void)
{
  GpioOutSet(LED_PORT, LED_PIN);
}

__STATIC_FORCEINLINE void LedOff(void)
{
  GpioOutReset(LED_PORT, LED_PIN);
}

#endif

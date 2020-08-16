// Includes --------------------------------------------------------------------
#include "gpio.h"

// Private Typedef -------------------------------------------------------------

// Private Macro ---------------------------------------------------------------

// Private Variables -----------------------------------------------------------

// Private Function prototypes -------------------------------------------------

// Functions -------------------------------------------------------------------
void GpioInit(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

  // Lock SWD Interfece
  GpioSetLock(SWDIO_GPIO_Port, SWDIO_Pin | SWCLK_Pin);

  // Иницилиция выводов для управления нагревателем и поджигом
  GpioSetMode(CTRL_GPIO_Port,
              HEAT_EN_Pin | FIRE_CHARGE_Pin,
              GPIO_MODE_OUTPUT_PUSHPULL,
              GPIO_SPEED_LOW);
  GpioOutReset(CTRL_GPIO_Port, HEAT_EN_Pin | FIRE_CHARGE_Pin);
  GpioSetMode(CTRL_GPIO_Port,
              FIRE_FAULT_Pin | FIRE_DONE_Pin | HEAT_FAULT_Pin,
              GPIO_MODE_INPUT,
              GPIO_SPEED_INPUT);
}

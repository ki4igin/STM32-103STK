// Includes --------------------------------------------------------------------
#include "gpio.h"

// Private Typedef -------------------------------------------------------------

// Private Macro ---------------------------------------------------------------

// Private Variables -----------------------------------------------------------

// Private Function prototypes -------------------------------------------------

// Functions -------------------------------------------------------------------
void GpioInit(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

  // Lock SWD Interfece
  GpioSetLock(SWDIO_GPIO_Port, SWDIO_Pin | SWCLK_Pin);

  GpioSetMode(LED_PORT, LED_PIN, GPIO_MODE_OUTPUT_PUSHPULL, GPIO_SPEED_LOW);
}

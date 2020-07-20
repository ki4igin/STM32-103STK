// Includes --------------------------------------------------------------------
#include "main.h"
#include "gpio.h"
#include "uart.h"
#include "rcc.h"
#include "tim.h"
#include "modbus.h"
// Private Typedef -------------------------------------------------------------

// Private Macro ---------------------------------------------------------------

// Private Variables -----------------------------------------------------------

// Private Function prototypes -------------------------------------------------
void SystemClock_Config(void);
void Delay_ms(uint32_t delay);

// Functions -------------------------------------------------------------------
int main(void)
{
  // MCU Configuration
  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  // System interrupt init
  // Configure the system clock
  RccInit();
  SysTickInit();
  // Initialize all configured peripherals
  GpioInit();
  ModBusInit();  
  UsartSendStrIT(USART1, (uint8_t*)"\nInit Complete\n");
  

  // Infinite loop
  while (1)
  {
    ModBusProcess();
    LedOn();
    Delay_ms(1000);
    LedOff();
    Delay_ms(1000);
  }
}

// void Tim2Update_Callback(void)
// {
//   static uint16_t cnt;
//   UsartSendByte(LPUART1, (uint8_t)(cnt >> 8));
//   UsartSendByte(LPUART1, (uint8_t)cnt);
//   cnt++;
// }

// void Tim6Update_Callback(void)
// {
// }

void Delay_ms(uint32_t delay)
{
  // __IO uint32_t temp = SysTick->CTRL;
  delay++;
  while (delay)
  {
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    {
      delay--;
    }
  }
}

void Error_Handler(void)
{
  while (1)
  {
    LedOn();
    Delay_ms(300);
    LedOff();
    Delay_ms(300);
  }
}

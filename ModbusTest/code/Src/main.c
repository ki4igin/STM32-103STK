// Includes --------------------------------------------------------------------
#include "main.h"
#include "gpio.h"
#include "uart.h"
#include "rcc.h"
#include "tim.h"
#include "adc.h"
#include "dma.h"
#include "modbus.h"
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_dma.h"
#include "AD5270.h"
// Private Typedef -------------------------------------------------------------

// Private Macro ---------------------------------------------------------------

// Private Variables -----------------------------------------------------------

// Private Function prototypes -------------------------------------------------
void                 SystemClock_Config(void);
__STATIC_INLINE void RegsUpdate(void);
__STATIC_INLINE void AdcUpdate(void);
// Functions
// -------------------------------------------------------------------
int main(void)
{
  // MCU Configuration
  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  // System interrupt init

  /** NOJTAG: JTAG-DP Disabled and SW-DP Enabled
   */
  AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

  // Configure the system clock
  RccInit();
  SysTickInit();
  // Initialize all configured peripherals
  Delay_ms(100);
  GpioInit();
  ModBusInit();
  MX_DMA_Init();
  MX_ADC1_Init();
  TIM3_Init();
  AD5270Init();

  // AD5270WriteRdac(SLA_FIRE, 0x100);
  DebugSendMessage("Init Complete\n");

  LL_ADC_Enable(ADC1);
  LL_ADC_REG_StartConversionExtTrig(ADC1, LL_ADC_REG_TRIG_EXT_RISING);
  LL_TIM_EnableCounter(TIM3);
  // Infinite loop
  while (1)
  {
    ModBusProcess();
    if (mbFlags.regsUpdate)
    {
      mbFlags.regsUpdate = 0;
      RegsUpdate();
    }
    if (adcFlags.adcUpdate)
    {
      adcFlags.adcUpdate = 0;
      AdcUpdate();
    }
  }
}

__STATIC_INLINE void RegsUpdate()
{
  if (mbHoldRegs.SCR.fireOn)
  {
    GpioOutSet(CTRL_GPIO_Port, FIRE_CHARGE_Pin);
  }
  else
  {
    GpioOutReset(CTRL_GPIO_Port, FIRE_CHARGE_Pin);
  }
  if (mbHoldRegs.SCR.HeatOn)
  {
    GpioOutSet(CTRL_GPIO_Port, HEAT_EN_Pin);
  }
  else
  {
    GpioOutReset(CTRL_GPIO_Port, HEAT_EN_Pin);
  }

  AD5270WriteRdac(SLA_HEAT, mbHoldRegs.HRC);
}

__STATIC_INLINE void AdcUpdate()
{
  mbInRegs.FMC = adcDate.fireMonCur;
  mbInRegs.FMV = adcDate.fireMonV;
  mbInRegs.HMC = adcDate.heatMonCur;
  mbInRegs.HMV = adcDate.heatMonV;
  mbInRegs.TMG = adcDate.temp;

  if (mbHoldRegs.TMS < mbInRegs.TMG)
  {
    GpioOutReset(CTRL_GPIO_Port, HEAT_EN_Pin);
    GpioOutReset(CTRL_GPIO_Port, FIRE_CHARGE_Pin);
    mbHoldRegs.SCR.HeatOn = 0;
    mbHoldRegs.SCR.fireOn = 0;
    mbInRegs.TCC |= 0x02;
  }
  else
  {
    mbInRegs.TCC &= ~0x02;
  }
  
  
}

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

#ifdef DEBUG
void DebugSendMessage(char* str)
{
  UsartSendStrIT(USART1, (uint8_t*)str);
}
#else
void DebugSendMessage(char* str)
{
  (void)str;
}
#endif

void Error_Handler(void)
{
  while (1)
  {
  }
}


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_adc.h"
#include "uart.h"
/* Private includes ----------------------------------------------------------*/

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
  if (LL_DMA_IsActiveFlag_HT1(DMA1))
  {
    LL_DMA_ClearFlag_HT1(DMA1);
    Dma1Channel1HT_Callback();
  }
}

void ADC1_2_IRQHandler(void)
{
  if (LL_ADC_IsActiveFlag_EOS(ADC1))
  {
    ;
  }
}

void USART1_IRQHandler(void)
{
  if (((USART1->SR & USART_SR_RXNE) == USART_SR_RXNE) &&
      ((USART1->CR1 & USART_CR1_RXNEIE) == USART_CR1_RXNEIE))
  {
    Usart1Rx_Callback();
  }
  if (((USART1->SR & USART_SR_TC) == USART_SR_TC) &&
      ((USART1->CR1 & USART_CR1_TCIE) == USART_CR1_TCIE))
  {
    USART1->SR &= ~USART_SR_TC;
  }
  if (((USART1->SR & USART_SR_TXE) == USART_SR_TXE) &&
      ((USART1->CR1 & USART_CR1_TXEIE) == USART_CR1_TXEIE))
  {
    Usart1Tx_Callback();
  }
  if ((USART1->SR & USART_SR_ORE) == USART_SR_ORE)
  {
    (void)USART1->SR;
    (void)USART1->DR;
  }
}

void TIM4_IRQHandler(void)
{
  if (LL_TIM_IsActiveFlag_UPDATE(TIM4))
  {
    LL_TIM_ClearFlag_UPDATE(TIM4);
    Tim4Update_Callback();
  }
  if (LL_TIM_IsActiveFlag_CC1(TIM4))
  {
    LL_TIM_ClearFlag_CC1(TIM4);
    Tim4CC1_Callback();
  }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

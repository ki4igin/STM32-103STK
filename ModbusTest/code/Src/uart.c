// Includes --------------------------------------------------------------------
#include "uart.h"
#include "gpio.h"
#include "stm32f1xx.h"
#include "main.h"
// Private Typedef -------------------------------------------------------------

// Private Macro ---------------------------------------------------------------

#define UART_RX_PIN    RX_Pin
#define UART_TX_PIN    TX_Pin
#define UART_PORT      GPIOA
#define UART_BAUD_RATE 115200
// Private Variables -----------------------------------------------------------
UartFlags_t uartFlags = {0};

static uint8_t  cntRx     = {0};
static uint8_t  cnt15t    = {0};
static uint8_t  txBufSize = {0};
static uint8_t *ptxBuf;
static uint8_t *prxBuf;
// Private Function prototypes -------------------------------------------------

// Functions -------------------------------------------------------------------
void UsartInit(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

  NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_EnableIRQ(USART1_IRQn);

  // GPIO Configuration
  GpioSetMode(UART_PORT, UART_TX_PIN, GPIO_MODE_ALTERNATE_PUSHPULL, GPIO_SPEED_HIGH);
  GpioSetMode(UART_PORT, UART_RX_PIN, GPIO_MODE_FLOATING, GPIO_SPEED_INPUT);
  // UART Congiguration
  USART1->BRR = SystemCoreClock / UART_BAUD_RATE;
  USART1->CR1 = USART_CR1_M | USART_CR1_PCE | USART_CR1_TE | USART_CR1_RE;
  USART1->CR1 |= USART_CR1_UE;
}

void UsartSendStr(USART_TypeDef *USARTx, uint8_t *str)
{
  while (*str != 0)
  {
    while ((USARTx->SR & USART_SR_TXE) != USART_SR_TXE)
    {
      /* code */
    }

    USARTx->DR = *str++;
  }
}

void UsartSendArray(USART_TypeDef *USARTx, uint8_t *pbuf, uint8_t bufSize)
{
  for (uint8_t i = 0; i < bufSize; i++)
  {
    while ((USARTx->SR & USART_SR_TXE) != USART_SR_TXE)
    {
      /* code */
    }
    USARTx->DR = *pbuf++;
  }
}

void UsartSendStrIT(USART_TypeDef *USARTx, uint8_t *str)
{
  ptxBuf    = str;
  txBufSize = 0;
  while (*str != 0)
  {
    txBufSize++;
    str++;
  }
  USARTx->CR1 |= USART_CR1_TXEIE;
}

void UsartSendArrayIT(USART_TypeDef *USARTx, uint8_t *pbuf, uint8_t bufSize)
{
  if (txBufSize)
  {
    return;
  }
  txBufSize = bufSize;
  ptxBuf    = pbuf;
  USARTx->CR1 |= USART_CR1_TXEIE;
}

void UsartSendByte(USART_TypeDef *USARTx, uint8_t value)
{
  while ((USARTx->SR & USART_SR_TXE) != USART_SR_TXE)
  {
    /* code */
  }
  USARTx->DR = value;
}

void UsartReceiveIT(USART_TypeDef *USARTx, uint8_t *pbuf)
{
  if (cntRx)
  {
    return;
  }

  prxBuf = pbuf;
  USARTx->CR1 |= USART_CR1_RXNEIE;
}

void Usart1Rx_Callback()
{
  TIM4->CNT = 0;
  if (!cntRx)
  {
    TIM4->CR1 |= TIM_CR1_CEN;
  }

  *prxBuf++ = USART1->DR;
  cntRx++;
}

void Usart1Tx_Callback()
{
  if (txBufSize)
  {
    txBufSize--;
    USART1->DR = *ptxBuf++;
  }
  else
  {
    USART1->CR1 &= ~USART_CR1_TXEIE;
  }
}

void Tim4Update_Callback()
{
  CLEAR_BIT(TIM4->CR1, TIM_CR1_CEN);
  CLEAR_BIT(USART1->CR1, USART_CR1_RXNEIE);

  uartFlags.rx    = 1;
  uartFlags.cntRx = cntRx;
  uartFlags.cnt15t = cnt15t;

  cnt15t = 0;
  cntRx = 0;
}

void Tim4CC1_Callback()
{
  cnt15t++;
}
// End File --------------------------------------------------------------------
#ifndef __UART_H
#define __UART_H

// Includes --------------------------------------------------------------------
#include "stm32f1xx.h"
// Typedef ---------------------------------------------------------------------
typedef struct
{
  uint8_t rx : 1;  // Флаг приема данных
  uint8_t tx : 1;  // Флаг передачи данных
  uint8_t cntRx;   // колличество принятых данных
} UartFlags_t;
// Macro -----------------------------------------------------------------------

// Variables -------------------------------------------------------------------
extern UartFlags_t uartFlags;
// Function prototypes ---------------------------------------------------------
void UsartInit(void);
void UsartSendStr(USART_TypeDef *USARTx, uint8_t *str);
void UsartSendStrIT(USART_TypeDef *USARTx, uint8_t *str);
void UsartSendByte(USART_TypeDef *USARTx, uint8_t value);
void UsartSendArray(USART_TypeDef *USARTx, uint8_t *pbuf, uint8_t bufSize);
void UsartSendArrayIT(USART_TypeDef *USARTx, uint8_t *pbuf, uint8_t bufSize);
void UsartReceiveIT(USART_TypeDef *USARTx, uint8_t *pbuf);

void Usart1Rx_Callback(void);
void Usart1Tx_Callback(void);
void Tim4Update_Callback(void);
// Inline Functions ------------------------------------------------------------

#endif

// Includes --------------------------------------------------------------------
#include "modbus.h"
#include "main.h"
#include "crc16.h"
#include "uart.h"
#include "tim.h"

// Private Typedef -------------------------------------------------------------

// Private Macro ---------------------------------------------------------------
#define UART LPUART

#define SLA ('1')
// Private Variables -----------------------------------------------------------
uint8_t rxBuf[256] = {0};
uint8_t txBuf[256] = {0};
// Private Function prototypes -------------------------------------------------
uint8_t DataProcessing(void);
// Functions -------------------------------------------------------------------
void ModBusInit()
{
  UsartInit();
  TIM4_Init();

  txBuf[0] = SLA;

  UsartReceiveIT(USART1, rxBuf);

}

void ModBusProcess()
{
  if (uartFlags.rx)
  {
    uartFlags.rx      = 0;
    uint8_t rxBufSize = uartFlags.cntRx;
    if ((rxBuf[0] != SLA) || (rxBufSize < 4))
    {
      UsartSendStrIT(USART1, (uint8_t*)"SLA Error\n");
      UsartReceiveIT(USART1, rxBuf);
      return;
    }
    uint16_t crc16   = Crc16(rxBuf, rxBufSize - 2);
    uint16_t crc16rx = __REV16(*((uint16_t*)&rxBuf[rxBufSize - 2]));
    if (crc16rx != crc16)
    {
      UsartSendStrIT(USART1, (uint8_t*)"Crc Error\n");
      UsartReceiveIT(USART1, rxBuf);
      return;
    }
    uint8_t txBufSize = DataProcessing();
    UsartSendArrayIT(USART1, txBuf, txBufSize);
    UsartReceiveIT(USART1, rxBuf);
  }
}

uint8_t DataProcessing()
{
  uint8_t txBufSize = 2;
  txBuf[1]  = rxBuf[1];



  uint16_t crc16   = Crc16(txBuf, txBufSize);
  txBuf[txBufSize++] = (uint8_t)(crc16 >> 8);
  txBuf[txBufSize++] = (uint8_t)crc16;  
  return txBufSize;
}

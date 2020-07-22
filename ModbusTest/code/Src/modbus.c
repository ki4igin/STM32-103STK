// Includes --------------------------------------------------------------------
#include "modbus.h"
#include "main.h"
#include "crc16.h"
#include "uart.h"
#include "tim.h"

// Private Typedef -------------------------------------------------------------
typedef enum
{
  FUNC_READ_Coils       = 0x01,
  FUNC_READ_DiscInput   = 0x02,
  FUNC_READ_HoldRegs    = 0x03,
  FUNC_READ_InputRegs   = 0x04,
  FUNC_WRITE_SingleCoil = 0x05,
  FUNC_WRITE_SingleReg  = 0x06,
  FUNC_WRITE_MultCoils  = 0x0F,
  FUNC_WRITE_MultleRegs = 0x10,
} FUNC_TypeDef;
// Private Macro ---------------------------------------------------------------
#define CRC_CHECK_DISABLE

#define UART LPUART

#define SLA ('1')
// Private Variables -----------------------------------------------------------
uint8_t rxBuf[256] = {0};
uint8_t txBuf[256] = {0};

struct
{
  uint16_t temp1;
  uint16_t temp2;
  uint16_t temp3;
} modBusDiscInput;
union
{
  struct
  {
    uint16_t coil0 : 1;
    uint16_t coil1 : 1;
    uint16_t coil2 : 1;
    uint16_t coil3 : 1;
    uint16_t coil4 : 1;
    uint16_t coil5 : 1;
    uint16_t coil6 : 1;
    uint16_t coil7 : 1;
    uint16_t coil8 : 1;
    uint16_t coil9 : 1;
    uint16_t coil10 : 1;
    uint16_t coil11 : 1;
    uint16_t coil12 : 1;
    uint16_t coil13 : 1;
    uint16_t coil14 : 1;
    uint16_t coil15 : 1;
  };
  uint16_t all;
} modBusCoils;

struct
{
  uint16_t temp1;
  uint16_t temp2;
  uint16_t temp3;
} modBusInputRegs;
struct
{
  uint16_t temp1;
  uint16_t temp2;
  uint16_t temp3;
} modBusHoldRegs;

// Private Function prototypes -------------------------------------------------
uint8_t DataProcessing(void);
// Functions -------------------------------------------------------------------
void ModBusInit()
{
  UsartInit();
  TIM4_Init();

  txBuf[0] = SLA;

  UsartReceiveIT(USART1, rxBuf);

  modBusCoils.all = 0x0844;
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
#ifndef CRC_CHECK_DISABLE
    uint16_t crc16   = Crc16(rxBuf, rxBufSize - 2);
    uint16_t crc16rx = *((uint16_t*)&rxBuf[rxBufSize - 2]);
    if (crc16rx != crc16)
    {
      UsartSendStrIT(USART1, (uint8_t*)"Crc Error\n");
      UsartReceiveIT(USART1, rxBuf);
      return;
    }
#endif
    uint8_t txBufSize = DataProcessing();
    UsartSendArrayIT(USART1, txBuf, txBufSize);
    UsartReceiveIT(USART1, rxBuf);
  }
}

uint8_t DataProcessing()
{
  uint8_t txBufSize = 2;
  txBuf[1]          = rxBuf[1];
  FUNC_TypeDef func = (FUNC_TypeDef)rxBuf[1];
  switch (func)
  {
    case FUNC_READ_Coils:
    {
      uint16_t startAddr = __REV16(*(uint16_t*)&rxBuf[2]);
      uint16_t quality   = __REV16(*(uint16_t*)&rxBuf[4]);

      txBuf[txBufSize++] = (quality - 1) / 8 + 1;
      // txBuf[txBufSize] = 0;
      for (uint16_t i = 0; i < quality; i++)
      {
        if ((i & 0x7) == 0x0)
        {
          txBuf[++txBufSize] = 0;
        }

        uint32_t bitMask = 1U << (startAddr + i);
        txBuf[txBufSize] |= (READ_BIT(modBusCoils.all, bitMask) == bitMask) << (i & 0x7);
      }
      txBufSize++;
      // uint16_t* pCoils    = (uint16_t*)&modBusCoils;
      // pCoils += startAddr;
      // for (uint16_t i = 0; i < quality; i++)
      // {
      //   txBuf[txBufSize++] = *pCoils++;
      // }
      break;
    }
    case FUNC_READ_DiscInput:
      break;
    case FUNC_READ_HoldRegs:
      /* code */
      break;
    case FUNC_READ_InputRegs:
      /* code */
      break;
    case FUNC_WRITE_SingleCoil:
      /* code */
      break;
    case FUNC_WRITE_SingleReg:
      /* code */
      break;
    case FUNC_WRITE_MultCoils:
      /* code */
      break;
    case FUNC_WRITE_MultleRegs:
      /* code */
      break;

    default:
      break;
  }
  uint16_t crc16     = Crc16(txBuf, txBufSize);
  txBuf[txBufSize++] = (uint8_t)crc16;
  txBuf[txBufSize++] = (uint8_t)(crc16 >> 8);
  return txBufSize;
}

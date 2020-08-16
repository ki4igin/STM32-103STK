// Includes --------------------------------------------------------------------
#include "modbus.h"
#include "crc16.h"
#include "uart.h"
#include "tim.h"

// Private Typedef -------------------------------------------------------------
typedef enum
{
  FUNC_READ_HoldRegs    = 0x03,
  FUNC_READ_InRegs      = 0x04,
  FUNC_WRITE_SingleReg  = 0x06,
  FUNC_WRITE_MultleRegs = 0x10,
} FUNC_TypeDef;

// Private Macro ---------------------------------------------------------------
// #define CRC_CHECK_DISABLE  // Выключение проверки crc

#define SLA (0x0A)  // Адрес устройства
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

MBHoldRegs_t mbHoldRegs = {0};
MBInRegs_t   mbInRegs   = {0};
MBFlags_t    mbFlags    = {0};
// Private Function prototypes -------------------------------------------------
uint8_t DataProcessing(void);

__STATIC_INLINE uint8_t ReadInRegs(uint8_t txBufSize);
__STATIC_INLINE uint8_t ReadHoldRegs(uint8_t txBufSize);
__STATIC_INLINE uint8_t WriteSingleReg(uint8_t txBufSize);
__STATIC_INLINE uint8_t WriteMultleRegs(uint8_t txBufSize);

__STATIC_INLINE uint8_t Error(uint8_t exceptionCode);
// Functions
// -------------------------------------------------------------------
void ModBusInit()
{
  mbHoldRegs.TRS = 70;
  mbHoldRegs.TMS = 70;

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
      DebugSendMessage("SLA Error\n");
      UsartReceiveIT(USART1, rxBuf);
      return;
    }
#ifndef CRC_CHECK_DISABLE
    uint16_t crc16   = Crc16(rxBuf, rxBufSize - 2);
    uint16_t crc16rx = *((uint16_t*)&rxBuf[rxBufSize - 2]);
    if (crc16rx != crc16)
    {
      DebugSendMessage("Crc Error\n");
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
    case FUNC_READ_InRegs:
      txBufSize = ReadInRegs(txBufSize);
      break;
    case FUNC_READ_HoldRegs:
      txBufSize = ReadHoldRegs(txBufSize);
      break;
    case FUNC_WRITE_SingleReg:
      txBufSize = WriteSingleReg(txBufSize);
      break;
    case FUNC_WRITE_MultleRegs:
      txBufSize = WriteMultleRegs(txBufSize);
      break;
    default:
      txBufSize = Error(0x01);
      break;
  }
  uint16_t crc16     = Crc16(txBuf, txBufSize);
  txBuf[txBufSize++] = (uint8_t)crc16;
  txBuf[txBufSize++] = (uint8_t)(crc16 >> 8);
  return txBufSize;
}

__STATIC_INLINE uint8_t ReadInRegs(uint8_t txBufSize)
{
  uint16_t startAddr = __REV16(*(uint16_t*)&rxBuf[2]);
  uint16_t quality   = __REV16(*(uint16_t*)&rxBuf[4]);

  if (quality == 0 || quality > 0x007D)
  {
    return Error(0x03);
  }
  else if ((startAddr > (sizeof(MBInRegs_t) / (sizeof(uint16_t)))) ||
           ((uint32_t)startAddr + quality) >
               (sizeof(MBInRegs_t) / (sizeof(uint16_t))))
  {
    return Error(0x02);
  }

  txBuf[txBufSize++] = 2 * quality;

  uint16_t* pmbRegs = (uint16_t*)&mbInRegs + startAddr;
  uint16_t* ptxBuf  = (uint16_t*)&txBuf[txBufSize];

  for (uint8_t i = 0; i < quality; i++)
  {
    *ptxBuf++ = __REV16(*pmbRegs++);
  }
  txBufSize += 2 * quality;
  return txBufSize;
}

__STATIC_INLINE uint8_t ReadHoldRegs(uint8_t txBufSize)
{
  uint16_t startAddr = __REV16(*(uint16_t*)&rxBuf[2]);
  uint16_t quality   = __REV16(*(uint16_t*)&rxBuf[4]);

  if (quality == 0 || quality > 0x007D)
  {
    return Error(0x03);
  }
  else if ((startAddr > (sizeof(MBHoldRegs_t) / (sizeof(uint16_t)))) ||
           ((uint32_t)startAddr + quality) >
               (sizeof(MBHoldRegs_t) / (sizeof(uint16_t))))
  {
    return Error(0x02);
  }

  txBuf[txBufSize++] = 2 * quality;

  uint16_t* pmbRegs = (uint16_t*)&mbHoldRegs + startAddr;
  uint16_t* ptxBuf  = (uint16_t*)&txBuf[txBufSize];

  for (uint8_t i = 0; i < quality; i++)
  {
    *ptxBuf++ = __REV16(*pmbRegs++);
  }
  txBufSize += 2 * quality;
  return txBufSize;
}

__STATIC_INLINE uint8_t WriteSingleReg(uint8_t txBufSize)
{
  uint16_t startAddr = __REV16(*(uint16_t*)&rxBuf[2]);
  uint16_t value     = __REV16(*(uint16_t*)&rxBuf[4]);

  if (startAddr > (sizeof(MBHoldRegs_t) / (sizeof(uint16_t))))
  {
    return Error(0x02);
  }
  uint16_t* pmbRegs  = (uint16_t*)&mbHoldRegs + startAddr;
  *pmbRegs           = value;
  txBuf[txBufSize++] = startAddr >> 8;
  txBuf[txBufSize++] = (uint8_t)startAddr;
  txBuf[txBufSize++] = value >> 8;
  txBuf[txBufSize++] = (uint8_t)value;

  mbFlags.regsUpdate = 1;

  return txBufSize;
}

__STATIC_INLINE uint8_t WriteMultleRegs(uint8_t txBufSize)
{
  uint16_t startAddr = __REV16(*(uint16_t*)&rxBuf[2]);
  uint16_t quality   = __REV16(*(uint16_t*)&rxBuf[4]);
  uint8_t  byteCnt   = rxBuf[6];

  if ((quality == 0 || quality > 0x007B) || (byteCnt != 2 * quality))
  {
    return Error(0x03);
  }
  else if ((startAddr > (sizeof(MBHoldRegs_t) / (sizeof(uint16_t)))) ||
           ((uint32_t)startAddr + quality) >
               (sizeof(MBHoldRegs_t) / (sizeof(uint16_t))))
  {
    return Error(0x02);
  }

  uint16_t* pmbRegs = (uint16_t*)&mbHoldRegs + startAddr;
  uint16_t* prxBuf  = (uint16_t*)&rxBuf[7];

  for (uint8_t i = 0; i < quality; i++)
  {
    *pmbRegs++ = __REV16(*prxBuf++);
  }

  txBuf[txBufSize++] = startAddr >> 8;
  txBuf[txBufSize++] = (uint8_t)startAddr;
  txBuf[txBufSize++] = 0;
  txBuf[txBufSize++] = (uint8_t)quality;

  mbFlags.regsUpdate = 1;

  return txBufSize;
}

__STATIC_INLINE uint8_t Error(uint8_t exceptionCode)
{
  txBuf[1] |= 0x80;
  txBuf[2] = exceptionCode;
  return 3;
}

// Includes --------------------------------------------------------------------
#include "AD5270.h"
// Private Typedef -------------------------------------------------------------
typedef enum
{
  COMMAND_NOP        = 0,
  COMMAND_WRITE_RDAC = 1,
  COMMAND_READ_RDAC  = 2,
} COMMAND_t;

// Private Macro ---------------------------------------------------------------

// Private Variables -----------------------------------------------------------

// Private Function prototypes -------------------------------------------------
uint16_t AD5270Send(SLA_t sla, COMMAND_t command, uint16_t data);
// Functions -------------------------------------------------------------------
void AD5270Init()
{
  SpiInit();
}

void AD5270WriteRdac(SLA_t sla, uint16_t rdac)
{
  __IO uint16_t temp = AD5270Send(sla, COMMAND_WRITE_RDAC, rdac);
  (void)temp;
}

uint16_t AD5270ReadRdac(SLA_t sla)
{
  __IO uint16_t temp =  AD5270Send(sla, COMMAND_WRITE_RDAC, 0x00);
  (void)temp;
  uint16_t rdac = AD5270Send(sla, COMMAND_WRITE_RDAC, 0x00);
  return rdac;
}

uint16_t AD5270Send(SLA_t sla, COMMAND_t command, uint16_t data)
{
  uint16_t dataRx = {0};
  uint16_t dataTx = (command << 10) | (data & 0x3FF);
  switch (sla)
  {
    case SLA_HEAT:
      dataRx = SpiCs1TxRx(dataTx);
      break;
    case SLA_FIRE:
      dataRx = SpiCs2TxRx(dataTx);
      break;
    default:
      break;
  }
  return dataRx;
}

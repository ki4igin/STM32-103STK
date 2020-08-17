#ifndef __MODBUS_H
#define __MODBUS_H

// Includes --------------------------------------------------------------------
#include "stm32f1xx.h"
// Typedef ---------------------------------------------------------------------
typedef struct
{
  uint16_t fireOn : 1;  // 1 – ПЭ включен6; 0 – ПЭ выключен
  uint16_t HeatOn : 1;  // 1 – Нг включен; 0 – Нг выключен
  uint16_t RESERVED : 14;

} MBRegsSCR_t;

typedef struct
{
  MBRegsSCR_t SCR;  // 0x0000	R/W	Регистр состояния и управления ПЭ и Нг
  uint16_t    HRC;  // 0x0001	R/W	Заданный ток Нг, 100*мА
  uint16_t    TRS;  // 0x0002	R/W	Максимальная температура радиатора, °С (после
                    // сброса – 70 °С)
  uint16_t TMS;  // 0x0003	R/W	Максимальная температура микроконтроллера, °С
                 // (после сброса – 70 °С)
} MBHoldRegs_t;

typedef struct
{
  uint16_t FMV;  // 0x0000	R	Измеренное напряжение ПЭ, В
  uint16_t FMC;  // 0x0001	R	Измеренный ток ПЭ, мА
  uint16_t HMC;  // 0x0002	R	Измеренный ток Нг, мА
  uint16_t HMV;  // 0x0003	R	Измеренное напряжение Нг, мВ
  uint16_t TCC;  // 0x0004	R	Регистр состояния температуры
  uint16_t TMG;  // 0x0006	R	Измеренная температура микроконтроллера, °С
  uint16_t TRG;  // 0x0005	R	Измеренная температура радиатора, °С
} MBInRegs_t;

typedef struct
{
  uint8_t regsUpdate : 1;
} MBFlags_t;
// Macro -----------------------------------------------------------------------

// Variables -------------------------------------------------------------------
extern MBHoldRegs_t mbHoldRegs;
extern MBInRegs_t   mbInRegs;
extern MBFlags_t    mbFlags;
// Function prototypes ---------------------------------------------------------
void ModBusInit(void);
void ModBusProcess(void);
// Inline Functions ------------------------------------------------------------

#endif

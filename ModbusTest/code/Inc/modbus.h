#ifndef __MODBUS_H
#define __MODBUS_H

// Includes --------------------------------------------------------------------
#include "stm32f1xx.h"
// Typedef ---------------------------------------------------------------------
typedef struct
{
  uint16_t fireOn : 1;  // 1 � �� �������6; 0 � �� ��������
  uint16_t HeatOn : 1;  // 1 � �� �������; 0 � �� ��������
  uint16_t RESERVED : 14;

} MBRegsSCR_t;

typedef struct
{
  MBRegsSCR_t SCR;  // 0x0000	R/W	������� ��������� � ���������� �� � ��
  uint16_t    HRC;  // 0x0001	R/W	�������� ��� ��, 100*��
  uint16_t    TRS;  // 0x0002	R/W	������������ ����������� ���������, �� (�����
                    // ������ � 70 ��)
  uint16_t TMS;  // 0x0003	R/W	������������ ����������� ����������������, ��
                 // (����� ������ � 70 ��)
} MBHoldRegs_t;

typedef struct
{
  uint16_t FMV;  // 0x0000	R	���������� ���������� ��, �
  uint16_t FMC;  // 0x0001	R	���������� ��� ��, ��
  uint16_t HMC;  // 0x0002	R	���������� ��� ��, ��
  uint16_t HMV;  // 0x0003	R	���������� ���������� ��, ��
  uint16_t TCC;  // 0x0004	R	������� ��������� �����������
  uint16_t TMG;  // 0x0006	R	���������� ����������� ����������������, ��
  uint16_t TRG;  // 0x0005	R	���������� ����������� ���������, ��
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

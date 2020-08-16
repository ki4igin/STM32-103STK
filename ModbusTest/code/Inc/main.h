#ifndef __MAIN_H
#define __MAIN_H

// Includes --------------------------------------------------------------------
#include "stm32f1xx.h"
// Typedef ---------------------------------------------------------------------

// Macro -----------------------------------------------------------------------


#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0 ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1 ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2 ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3 ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4 ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

// Variables -------------------------------------------------------------------

// Function prototypes ---------------------------------------------------------
void Error_Handler(void);
void DebugSendMessage(char* str);
void Delay_ms(uint32_t delay);
// Inline Functions ------------------------------------------------------------

#endif

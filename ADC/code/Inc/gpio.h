/**
 ******************************************************************************
 * @file    gpio.h
 * @brief   This file contains all the function prototypes for
 *          the gpio.c file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes
 * ------------------------------------------------------------------*/
#include "main.h"

#define LCD_D_C_Pin       LL_GPIO_PIN_2
#define LCD_D_C_GPIO_Port GPIOB
#define LCD_RES_Pin       LL_GPIO_PIN_7
#define LCD_RES_GPIO_Port GPIOC
#define SWDIO_Pin         LL_GPIO_PIN_13
#define SWDIO_GPIO_Port   GPIOA
#define SWCLK_Pin         LL_GPIO_PIN_14
#define SWCLK_GPIO_Port   GPIOA
#define LCD_E_Pin         LL_GPIO_PIN_10
#define LCD_E_GPIO_Port   GPIOC

  void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

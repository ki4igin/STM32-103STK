/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "stm32f1xx_ll_adc.h"
#include "gpio.h"
#include "stm32f1xx_ll_dma.h"

#define ADC_DELAY_ENABLE_CALIB_CPU_CYCLES                                      \
  (LL_ADC_DELAY_ENABLE_CALIB_ADC_CYCLES * 32)
/* Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */
#define INTERNAL_TEMPSENSOR_AVGSLOPE ((int32_t)4300)
/* Internal temperature sensor, parameter V25 (unit: mV). Refer to device datasheet for min/typ/max values. */
#define INTERNAL_TEMPSENSOR_V25      ((int32_t)1430)
#define INTERNAL_TEMPSENSOR_V25_TEMP ((int32_t)25)
#define INTERNAL_TEMPSENSOR_V25_VREF ((int32_t)3300)
#define VDDA_APPLI                   ((uint32_t)3300)

/* USER CODE BEGIN 0 */
typedef struct
{
  uint16_t fireMonCur;
  uint16_t heatMonV;
  uint16_t heatMonCur;
  uint16_t fireMonV;
  uint16_t temp;
  uint16_t vref;
} AdcDate_t;

/* USER CODE END 0 */
AdcDate_t adcDate   = {0};
uint16_t  adcBuf[6] = {0};

__STATIC_INLINE void ConvertData(void);
__STATIC_INLINE void Activate_ADC(void);
/* ADC1 init function */
void MX_ADC1_Init(void)
{
  LL_ADC_InitTypeDef       ADC_InitStruct       = {0};
  LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
  LL_ADC_REG_InitTypeDef   ADC_REG_InitStruct   = {0};

  /* Peripheral clock enable */
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;

  /**ADC1 GPIO Configuration  
  PA0-WKUP   ------> ADC1_IN0
  PA1   ------> ADC1_IN1
  PA2   ------> ADC1_IN2
  PA3   ------> ADC1_IN3 
  */
  GpioSetMode(MON_GPIO_Port,
              FIRE_MON_CUR_Pin | FIRE_MON_V_Pin | HEAT_MON_V_Pin |
                  HEAT_MON_CUR_Pin,
              GPIO_MODE_ANALOG,
              GPIO_SPEED_INPUT);

  /*## Configuration of NVIC #################################################*/
  /* Configure NVIC to enable ADC1 interruptions */
  NVIC_SetPriority(ADC1_IRQn, 0); /* ADC IRQ greater priority than DMA IRQ */
  NVIC_EnableIRQ(ADC1_IRQn);

  /* ADC1 DMA Init */

  LL_DMA_SetDataTransferDirection(DMA1,
                                  LL_DMA_CHANNEL_1,
                                  LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 6);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)adcBuf);
  LL_DMA_SetPeriphAddress(
      DMA1,
      LL_DMA_CHANNEL_1,
      LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA));
  LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_1);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

  /* ADC1 Init */
  /** Common config 
  */
  ADC_InitStruct.DataAlignment      = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_ENABLE;
  LL_ADC_Init(ADC1, &ADC_InitStruct);
  ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
  LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);

  ADC_REG_InitStruct.TriggerSource    = LL_ADC_REG_TRIG_EXT_TIM3_TRGO;
  ADC_REG_InitStruct.SequencerLength  = LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode   = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer      = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  /** Configure Regular Channel 
  */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
  LL_ADC_SetChannelSamplingTime(ADC1,
                                LL_ADC_CHANNEL_0,
                                LL_ADC_SAMPLINGTIME_1CYCLE_5);
  /** Configure Regular Channel 
  */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_1);
  LL_ADC_SetChannelSamplingTime(ADC1,
                                LL_ADC_CHANNEL_1,
                                LL_ADC_SAMPLINGTIME_1CYCLE_5);
  /** Configure Regular Channel 
  */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_2);
  LL_ADC_SetChannelSamplingTime(ADC1,
                                LL_ADC_CHANNEL_2,
                                LL_ADC_SAMPLINGTIME_1CYCLE_5);
  /** Configure Regular Channel 
  */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_4, LL_ADC_CHANNEL_3);
  LL_ADC_SetChannelSamplingTime(ADC1,
                                LL_ADC_CHANNEL_3,
                                LL_ADC_SAMPLINGTIME_1CYCLE_5);
  /** Configure Regular Channel 
  */
  LL_ADC_REG_SetSequencerRanks(ADC1,
                               LL_ADC_REG_RANK_5,
                               LL_ADC_CHANNEL_TEMPSENSOR);
  LL_ADC_SetChannelSamplingTime(ADC1,
                                LL_ADC_CHANNEL_TEMPSENSOR,
                                LL_ADC_SAMPLINGTIME_239CYCLES_5);
  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
                                 LL_ADC_PATH_INTERNAL_TEMPSENSOR);
  /** Configure Regular Channel 
  */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_6, LL_ADC_CHANNEL_VREFINT);
  LL_ADC_SetChannelSamplingTime(ADC1,
                                LL_ADC_CHANNEL_VREFINT,
                                LL_ADC_SAMPLINGTIME_239CYCLES_5);
  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
                                 LL_ADC_PATH_INTERNAL_VREFINT);

  Activate_ADC();
}

__STATIC_INLINE void Activate_ADC(void)
{
  __IO uint32_t wait_loop_index = 0;
#if (USE_TIMEOUT == 1)
  uint32_t Timeout = 0; /* Variable used for timeout management */
#endif                  /* USE_TIMEOUT */

  /*## Operation on ADC hierarchical scope: ADC instance #####################*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features are not             */
  /*       conditioned to ADC state.                                          */
  /*       However, in order to be compliant with other STM32 series          */
  /*       and to show the best practice usages, ADC state is checked.        */
  /*       Software can be optimized by removing some of these checks, if     */
  /*       they are not relevant considering previous settings and actions    */
  /*       in user application.                                               */
  if (LL_ADC_IsEnabled(ADC1) == 0)
  {
    /* Enable ADC */
    LL_ADC_Enable(ADC1);

    /* Delay between ADC enable and ADC start of calibration.                 */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    wait_loop_index = (ADC_DELAY_ENABLE_CALIB_CPU_CYCLES >> 1);
    while (wait_loop_index != 0)
    {
      wait_loop_index--;
    }

    /* Run ADC self calibration */
    LL_ADC_StartCalibration(ADC1);

/* Poll for ADC effectively calibrated */
#if (USE_TIMEOUT == 1)
    Timeout = ADC_CALIBRATION_TIMEOUT_MS;
#endif /* USE_TIMEOUT */

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
#if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if (Timeout-- == 0)
        {
          /* Time-out occurred. Set LED to blinking mode */
          LED_Blinking(LED_BLINK_ERROR);
        }
      }
#endif /* USE_TIMEOUT */
    }
  }
}

void Dma1Channel1HT_Callback()
{
  ConvertData();
}

__STATIC_INLINE void ConvertData(void)
{
  adcDate.fireMonCur = adcBuf[0];
  adcDate.heatMonV   = adcBuf[1];
  adcDate.heatMonCur = adcBuf[2];
  adcDate.fireMonV   = adcBuf[3];
  adcDate.temp =
      __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(INTERNAL_TEMPSENSOR_AVGSLOPE,
                                           INTERNAL_TEMPSENSOR_V25,
                                           INTERNAL_TEMPSENSOR_V25_TEMP,
                                           VDDA_APPLI,
                                           adcBuf[4],
                                           LL_ADC_RESOLUTION_12B);
  adcDate.vref = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI,
                                               adcBuf[5],
                                               LL_ADC_RESOLUTION_12B);

  // uint16_t fireMonCur;
  // uint16_t heatMonV;
  // uint16_t heatMonCur;
  // uint16_t fireMonV;
  // uint16_t temp;
  // uint16_t vref;
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
  ******************************************************************************
  * @file    KEY.h
  * @author  HY R&D Team
  * @version v1.0
  * @date    3-July-2018
  * @brief   Header file for KEY.c module.
  ******************************************************************************
  * @attention
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, HOYEN TECH SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2018 HOYEN TECH Co., Ltd </center></h2>
  ******************************************************************************
  */

#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Header includes -----------------------------------------------------------*/
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stm32f0xx.h"
/* Macro definitions ---------------------------------------------------------*/
/*          X1     X2     X3         */
/*  ---------------------------------*/
/*   Y1 |  Key1   Key2   Key3        */
/*   Y2 |  Key4   Key5   Key6        */
/*   Y3 |  Key7   Key8   Key9        */
/*   Y4 |  KeyA   Key0   KeyB        */
/*  ---------------------------------*/
#define COL_NUM                            (3)
#define ROW_NUM                            (3)
#define KEY_NUM                       (COL_NUM*ROW_NUM)
  
#define KeyRowHigh( Port,Pin )        GPIO_SetBits(Port, Pin)                /*!< Turns the given LED Off                     */
#define KeyRowLow( Port,Pin )         GPIO_ResetBits(Port,Pin)
#define KeyReadColHigh( Port,Pin )    ((GPIO_ReadInputDataBit(Port,Pin)== SET)? true: false)
#define KeyReadColLow( Port,Pin )     ((GPIO_ReadInputDataBit(Port,Pin)== RESET)? true: false)
#define X1_GPIO_PORT                  GPIOC
#define X1_GPIO_Pin                   GPIO_Pin_10
#define X1_EXTI_IRQn                  EXTI4_15_IRQn
#define X2_GPIO_PORT                  GPIOC
#define X2_GPIO_Pin                   GPIO_Pin_11
#define X2_EXTI_IRQn                  EXTI4_15_IRQn
#define X3_GPIO_PORT                  GPIOC
#define X3_GPIO_Pin                   GPIO_Pin_12
#define X3_EXTI_IRQn                  EXTI4_15_IRQn

#define Y1_GPIO_PORT                  GPIOB
#define Y1_GPIO_Pin                   GPIO_Pin_13
#define Y2_GPIO_PORT                  GPIOB
#define Y2_GPIO_Pin                   GPIO_Pin_14
#define Y3_GPIO_PORT                  GPIOB
#define Y3_GPIO_Pin                   GPIO_Pin_15
//#define Y4_GPIO_PORT                  GPIOB
//#define Y4_GPIO_Pin                   GPIO_Pin_15

#define KEY_PRESS_STATUS                Bit_SET

/* Type definitions ----------------------------------------------------------*/
typedef enum
{
  KEY1 = 0,
  KEY2,
  KEY3,
  KEY4,
  KEY5,
  KEY6,
  KEY7,
  KEY8,
  KEY9,
  KEY0,
  KEYA,
  KEYB,
  KEY_None
}KEY_Type;

typedef enum
{
  KEY_NoPress    = 0,
  KEY_ShortPress = 1,
  KEY_LongPress  = 2
}KEY_Status;

typedef enum
{
  PUSLE_ROW1 = 0,
  PUSLE_ROW2,
  PUSLE_ROW3,
  PUSLE_ROW4
}KEY_SCAN_STEP;

/* Variable declarations -----------------------------------------------------*/
typedef struct
{
  bool keyPressedScan;
  bool keyUpScan;
  bool keyEXTI;

  uint8_t keyInputHighCount[KEY_NUM];
  uint16_t keyScanCounter;
  uint16_t keyUpCounter;
  KEY_Status keyStatus[KEY_NUM];
  KEY_Type LastKeyPressed;
  void (*keyShortPressCallback[KEY_NUM]);
  void (*keyLongPressCallback[KEY_NUM]);
}KEY_STRUCTURE;

/* Variable definitions ------------------------------------------------------*/
/* Function declarations -----------------------------------------------------*/
void Key_Init(void);
void Key_DeInit(void);

KEY_Type GetKey(void);

void KEY_SetShortPressCallback(KEY_Type key, void (*fun)(void));
void KEY_SetLongPressCallback(KEY_Type key, void (*fun)(void));

void KEY_ClearShortPressCallback(KEY_Type key);
void KEY_ClearLongPressCallback(KEY_Type key);

void KeyScanHandler(void);


/* Function definitions ------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H */

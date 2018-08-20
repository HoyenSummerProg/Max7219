/**
  ******************************************************************************
  * @file    key.c
  * @author  HY R&D Team
  * @version V1.0
  * @date    07/19/2018
  * @brief   This file provides M*N key scan for system
  ******************************************************************************
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
*/

/* Header includes -----------------------------------------------------------*/
#include "key.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

/* Macro definitions ---------------------------------------------------------*/
/* Type definitions ----------------------------------------------------------*/
/* Variable declarations -----------------------------------------------------*/
/* Variable definitions ------------------------------------------------------*/
static uint8_t KeyMatrix[COL_NUM][ROW_NUM] = {{KEY1, KEY2, KEY3}, {KEY4, KEY5, KEY6}, {KEY7, KEY8, KEY9}};
GPIO_TypeDef* ROW_GPIO_PORT[ROW_NUM] = { Y1_GPIO_PORT, Y2_GPIO_PORT,Y3_GPIO_PORT};
GPIO_TypeDef* COL_GPIO_PORT[COL_NUM] = { X1_GPIO_PORT, X2_GPIO_PORT,X3_GPIO_PORT};

const uint32_t ROW_GPIO_Pin[ROW_NUM] = {Y1_GPIO_Pin, Y2_GPIO_Pin, Y3_GPIO_Pin};
const uint32_t COL_GPIO_Pin[COL_NUM] = {X1_GPIO_Pin, X2_GPIO_Pin, X3_GPIO_Pin};

__IO KEY_SCAN_STEP keyScanStep = PUSLE_ROW1;
__IO uint8_t Key_Col = 0;
__IO uint8_t Key_Row = 0;
__IO KEY_STRUCTURE keyStruc;
/* Function declarations -----------------------------------------------------*/
static void SetKeyColumnAsInput(void);
static void SetKeyColumnAsEXTI(void);
static void ResetKeyStructure(void);
/* Function declarations -----------------------------------------------------*/

/**
  * @brief  Key initializes.
  * @param  None.
  * @return None.
  */
void Key_Init(void)
{ 
  SetKeyColumnAsEXTI();
  ResetKeyStructure();
  /* TODO :config Key Timmer */
}

/**
  * @brief  Key de-initializes.
  * @param  None.
  * @return None.
  */
void Key_DeInit(void)
{
  //TODO:
  
}

/**
  * @brief  Get key status.
  * @param  [in] pin: That key.
  * @return Key status.
  */
KEY_Type GetKey(void)
{
  KEY_Type key = keyStruc.LastKeyPressed;

  keyStruc.keyStatus[key] = KEY_NoPress;
  keyStruc.LastKeyPressed = KEY_None;

  return key;
}


/**
  * @brief  Set key short press callback.
  * @param  [in] pin: That key.
  * @param  [in] fun: Function pointer.
  * @return None.
  */
void KEY_SetShortPressCallback(KEY_Type key, void (*fun)(void))
{
  keyStruc.keyShortPressCallback[key] = (__IO void (*)(void))fun;
}


/**
  * @brief  Set key long press callback.
  * @param  [in] pin: That key.
  * @param  [in] fun: Function pointer.
  * @return None.
  */
void KEY_SetLongPressCallback(KEY_Type key, void (*fun)(void))
{
  keyStruc.keyLongPressCallback[key] = (__IO void (*)(void))fun;
}


/**
  * @brief  Clear key short press callback.
  * @param  [in] pin: That key.
  * @param  [in] fun: Function pointer.
  * @return None.
  */
void KEY_ClearShortPressCallback(KEY_Type key)
{
  keyStruc.keyShortPressCallback[key] = NULL;
}


/**
  * @brief  Clear key long press callback.
  * @param  [in] pin: That key.
  * @param  [in] fun: Function pointer.
  * @return None.
  */
void KEY_ClearLongPressCallback(KEY_Type key)
{
  keyStruc.keyLongPressCallback[key] = NULL;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
    case GPIO_Pin_10:
      Key_Col = 0;
      keyStruc.keyEXTI = true;
    break;
    case GPIO_Pin_11:
      Key_Col = 1;
      keyStruc.keyEXTI = true;
    break;
    case GPIO_Pin_12:
      Key_Col = 2;
      keyStruc.keyEXTI = true;
    break;
    default:
      keyStruc.keyEXTI = false;
      break;
  }

  if(keyStruc.keyEXTI)
  {
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //TODO: check if remove EXTI is necessary
    SetKeyColumnAsInput();
    KeyRowLow(Y1_GPIO_PORT, Y1_GPIO_Pin);
    KeyRowLow(Y2_GPIO_PORT, Y2_GPIO_Pin);
    KeyRowLow(Y3_GPIO_PORT, Y3_GPIO_Pin);
//    KeyRowLow(Y4_GPIO_PORT, Y4_GPIO_Pin);
    TIM_Cmd(TIM3, ENABLE);
		TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    keyStruc.keyPressedScan = true;
    keyStruc.keyScanCounter = 0;
  }
}


void HAL_TIM_PeriodElapsedCallback(void)
{
  uint8_t i = 0;

	//key pressed check
	if(keyStruc.keyPressedScan)
	{
		switch(keyScanStep)
		{
			case PUSLE_ROW1:
				KeyRowLow(Y3_GPIO_PORT, Y3_GPIO_Pin);
				KeyRowHigh(Y1_GPIO_PORT, Y1_GPIO_Pin);
				break;
			case PUSLE_ROW2:
				KeyRowLow(Y1_GPIO_PORT, Y1_GPIO_Pin);
				KeyRowHigh(Y2_GPIO_PORT, Y2_GPIO_Pin);
				break;
			case PUSLE_ROW3:
				KeyRowLow(Y2_GPIO_PORT, Y2_GPIO_Pin);
				KeyRowHigh(Y3_GPIO_PORT, Y3_GPIO_Pin);
				break;
//			case PUSLE_ROW4:
//				KeyRowLow(Y3_GPIO_PORT, Y3_GPIO_Pin);
//				KeyRowHigh(Y4_GPIO_PORT, Y4_GPIO_Pin);
//				break;
			default:
				KeyRowLow(Y1_GPIO_PORT, Y1_GPIO_Pin);
				KeyRowLow(Y2_GPIO_PORT, Y2_GPIO_Pin);
				KeyRowLow(Y3_GPIO_PORT, Y3_GPIO_Pin);
//				KeyRowLow(Y4_GPIO_PORT, Y4_GPIO_Pin);
				break;
		}

		if(KeyReadColHigh(X1_GPIO_PORT, X1_GPIO_Pin))
			keyStruc.keyInputHighCount[keyScanStep]++;
		if(KeyReadColHigh(X2_GPIO_PORT, X2_GPIO_Pin))
			keyStruc.keyInputHighCount[keyScanStep + ROW_NUM]++;
		if(KeyReadColHigh(X3_GPIO_PORT, X3_GPIO_Pin))
			keyStruc.keyInputHighCount[keyScanStep + ROW_NUM*2]++;

		keyStruc.keyUpCounter++;

		if(keyScanStep < PUSLE_ROW3)
			keyScanStep++;
		else 
		{
			keyScanStep = PUSLE_ROW1;
			if(keyStruc.keyScanCounter++ > 3)
			{
				for(i = 0; i < KEY_NUM; i++)
				{
					if(keyStruc.keyInputHighCount[i] > 3)
					{
						keyStruc.LastKeyPressed = (KEY_Type)KeyMatrix[i / 3][i % 3];
						//TODO: add code to check key up
						Key_Col = i / 3;
						Key_Row = i % 3;
						KeyRowLow(Y3_GPIO_PORT, Y3_GPIO_Pin);
						KeyRowHigh(ROW_GPIO_PORT[Key_Row], ROW_GPIO_Pin[Key_Row]);
//              printf("key: %d\r\n", keyStruc.LastKeyPressed);
						keyStruc.keyPressedScan = false;
						keyStruc.keyUpScan = true;
						break;
					}
				}
			}
		}
	}
	else if (keyStruc.keyUpScan)
	{
		if(KeyReadColLow(COL_GPIO_PORT[Key_Col], COL_GPIO_Pin[Key_Col]))
		{
			/* Key up, set column pins as EXTI */
			if( keyStruc.keyUpCounter++ > 2)
			{
				TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
				TIM_Cmd(TIM3, DISABLE);
				TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);
				TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
				keyStruc.keyEXTI = false;
				keyStruc.keyUpCounter = 0;
				keyStruc.keyScanCounter = 0;
				memset((void*)&keyStruc.keyInputHighCount, 0, KEY_NUM);
				SetKeyColumnAsEXTI();
			}
		}
	}

}

static void SetKeyColumnAsInput(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /*Configure GPIO pins : COL_3_Pin COL_2_Pin COL_1_Pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
   /* Connect EXTI0 Line to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource12);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line10|EXTI_Line11|EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
}


static void SetKeyColumnAsEXTI(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
  /*Configure GPIO pins : COL_3_Pin COL_2_Pin COL_1_Pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* Connect EXTI0 Line to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource12);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line10|EXTI_Line11|EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void ResetKeyStructure(void)
{
  int i = 0;
  /* Key struc Init */
  for( i = 0; i < KEY_NUM; i++)
  {
    keyStruc.keyInputHighCount[i] = 0;
    keyStruc.keyLongPressCallback[i] = NULL;
    keyStruc.keyShortPressCallback[i] = NULL;
    keyStruc.keyStatus[i] = KEY_NoPress; 
  }

  keyStruc.keyEXTI = false;
  keyStruc.keyPressedScan = false;
  keyStruc.LastKeyPressed = KEY_None;
  keyStruc.keyScanCounter = 0;
  keyStruc.keyUpCounter = 0;
}

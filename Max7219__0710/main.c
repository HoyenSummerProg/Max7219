/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "hwconfig.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "time.h"
#include "key.h"
#include "stm32072b_eval.h"
/** @addtogroup STM32F0xx_StdPeriph_Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
SPI_InitTypeDef         SPI_InitStructure;
GPIO_InitTypeDef        GPIO_InitStructure;
EXTI_InitTypeDef   EXTI_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;
/* Private define ------------------------------------------------------------*/

#define HIGH 1
#define LOW  0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
static __IO uint32_t TimingDelay_ms;
uint16_t Buffer = 0x0000;
uint8_t trBuffer[8];
uint8_t count = 0;
extern __IO uint8_t num[10][8];
extern __IO uint8_t circle[4][7];
__IO uint32_t seed ;
__IO bool stflag = LOW;
__IO bool exflag = LOW;
__IO uint8_t btnflag;
__IO uint16_t CCR1_Val = 0;
uint16_t PrescalerValue = 1999;
__IO uint8_t timcount = 0;
__IO uint8_t x[8];
/* Private function prototypes -----------------------------------------------*/
//static void TIM_Config(void);
/* Private functions ---------------------------------------------------------*/
void SPI1_Config(void);
static void EXTI4_15_Config(void);
static void TIM_Config(void);
void SPICS1_Config(void);
void SPICS2_Config(void);
void Write_Max7219(unsigned char address,unsigned char data,uint8_t quantity);
void Write_Max7219_2(unsigned char address,unsigned char data,uint8_t quantity);
void Max7219_Init(uint8_t quantity);
void Max7219_2_Init(void);
void TimingDelay_ms_Decrement(void);
void Delay_ms(__IO uint32_t nTime_ms);
void Display(uint8_t chip ,uint8_t x);
void Display_2(uint8_t chip,uint8_t x);
void shift(uint8_t vect);
void clear(void);
void clear_2(void);
void runcircle(uint8_t t,uint8_t stop);
void setDot(uint8_t x,uint8_t y,bool status);
void scan(uint8_t line);
void keyscan(void);
void effect(uint8_t fa);
void dis_num(uint16_t input);
void GPIO_SCAN_Config(void);
void stop(uint8_t tos,uint8_t address,uint8_t now);
uint8_t transferangle(uint8_t number,uint8_t bit);
uint16_t ran(void);
__IO uint8_t t = 0x00;
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{	
	Key_Init();
	KEY_Type k;
	uint16_t a = 0;
	SPI1_Config();
	TIM_Config();
	SPICS1_Config();
	SPICS2_Config();
	GPIO_SCAN_Config();
//	EXTI4_15_Config();
	SPI_Cmd(SPI1, ENABLE);
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    while (1);
	}
	Delay_ms(50);
	Max7219_Init(1);
	Delay_ms(50);
	Max7219_2_Init();
  while (1)
  {
		k = GetKey();
		if(k!=KEY_None)
		{
			x[k]++;
			if(k+1>=9)
			{
				exflag=HIGH;
			}
			else
			{
			  Write_Max7219_2(k+1,x[k],1);
			}
		}
		a= ran();
		if(exflag == HIGH)
		{
			clear();
			for(int i=20;i<=300;i+=70)
	    {
	 	   runcircle(i,a);
	    }
			for(int i=0;i<8;i++)
			{
				x[i]=0;
			}
			exflag = LOW;
			stflag =LOW;
			clear_2();
			effect(a-84);
		}
  }
}
uint16_t ran(void)
{
	return (seed%28)+84 ;
}
void stop(uint8_t tos,uint8_t address,uint8_t now)
{
	count++;
	if(tos == count)
	{
		clear();
		count = 0;
		Write_Max7219(address,now,1);
		stflag = HIGH;
	}
}
void runcircle(uint8_t t,uint8_t st)
{
	for(int i=0;i<7;i++)
	{
		if(stflag==LOW)
		{
			clear();
			Write_Max7219(0x08,circle[0][i],1);
			stop(st,0x08,circle[0][i]);
			Delay_ms(t);
	  }
	}
	for(int i=0;i<7;i++)
	{
		if(stflag==LOW)
		{
			clear();
			Write_Max7219(8-i,circle[1][i],1);
			stop(st,8-i,circle[1][i]);
			Delay_ms(t);
		}
	}
	for(int i=0;i<7;i++)
	{
		if(stflag==LOW)
		{
			clear();
			Write_Max7219(0x01,circle[2][i],1);
			stop(st,0x01,circle[2][i]);
			Delay_ms(t);
		}
	}
	for(int i=0;i<7;i++)
	{
		if(stflag==LOW)
		{
			clear();
			Write_Max7219(i+1,circle[3][i],1);
			stop(st,i+1,circle[3][i]);
			Delay_ms(t);
		}
	}
}
void clear(void)
{
	for(int i=1;i<9;i++)
	{
		Write_Max7219(i,0x00,1);
		Write_Max7219(i,0x00,2);
		Write_Max7219(i,0x00,3);
		Write_Max7219(i,0x00,4);
	}
}
void effect(uint8_t fa)
{
	switch(fa)
	{
		case 1:
			dis_num(75*x[2]);
		  break;
		case 2:
			dis_num(100*x[6]);
		  break;
		case 3:
			dis_num(10*x[0]);
		  break;
		case 4:
			dis_num(100);
		  break;
		case 5:
			dis_num(25*x[7]);
		  break;
		case 6:
			dis_num(10*x[0]);
		  break;
		case 7:
			dis_num(50*x[1]);
		  break;
		case 8:
			dis_num(100*x[3]);
		  break;
		case 9:
			dis_num(10*x[0]);
		  break;
		case 10:
			dis_num(200*x[4]);
		  break;
		case 11:
			dis_num(25*x[7]);
		  break;
		case 12:
			dis_num(10*x[0]);
		  break;
		case 13:
			dis_num(75*x[2]);
		  break;
		case 14:
			dis_num(150*x[5]);
		  break;
		case 15:
			dis_num(10*x[0]);
		  break;
		case 16:
			dis_num(100);
		  break;
		case 17:
			dis_num(25*x[7]);
		  break;
		case 18:
			dis_num(10*x[0]);
		  break;
		case 19:
			dis_num(50*x[1]);
		  break;
		case 20:
			dis_num(100*x[3]);
		  break;
		case 21:
			dis_num(250);
		  break;
		case 22:
			dis_num(500);
		  break;
		case 23:
			dis_num(25*x[7]);
		  break;
		case 24:
			dis_num(10*x[0]);
		  break;
		default:
			break;
	}
}
void dis_num(uint16_t input)
{
	int a,b,c;
	if(input<10)
	{
		Write_Max7219_2(1,input,1);
	}
	else if((input>=10)&&(input<100))
	{
		a=input%10;
		b=input/10;
		Write_Max7219_2(2,b,1);
		Write_Max7219_2(1,a,1);
	}
	else if((input>=100)&&(input<1000))
	{
		a=input%10;
		b=input/10;
		c=b/10;
		Write_Max7219_2(3,c,1);
		Write_Max7219_2(2,b,1);
		Write_Max7219_2(1,a,1);
	}
}
void clear_2(void)
{
	for(int i=1;i<9;i++)
	{
		Write_Max7219_2(i,0x00,1);
	}
}
void Display(uint8_t chip,uint8_t x)
{
	for(int i=1;i<8;i++)
	{
	  Write_Max7219(i,transferangle(x,8-i),chip);
	}
}
void Display_2(uint8_t chip,uint8_t x)
{
	for(int i=1;i<8;i++)
	{
	  Write_Max7219_2(i,transferangle(x,8-i),chip);
	}
}
void shift(uint8_t vect)
{
	uint8_t s1[8];
	clear();
	for(int i=1;i<8;i++)
	{
		s1[i] = transferangle(1,8-i) << vect;
	  Write_Max7219(i,s1[i],1);
		if(vect>=4||transferangle(2,8-i)!=0x00)
	  {
		  s1[i]=transferangle(1,8-i)>>(8-vect);
		  Write_Max7219(i,s1[i],2);
	  }
	}
}
void setDot(uint8_t x,uint8_t y,bool status)
{
	uint8_t dBuffer[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	uint8_t r1;
	if(status == 1)
	{
		if(x<8)
		{
			r1=x%8;
		  Write_Max7219(y,dBuffer[r1],1);
		}
		else if(8<=x&&x<16)
		{
			r1=x%8;
			Write_Max7219(y,dBuffer[r1],2);
		}
		else if(16<=x&&x<24)
		{
			r1=x%8;
			Write_Max7219(y,dBuffer[r1],3);
		}
		else if(24<=x&&x<32)
		{
			r1=x%8;
			Write_Max7219(y,dBuffer[r1],4);
		}
	}
}
//static void EXTI4_15_Config(void)
//{
//  /* Enable GPIOA clock */
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//  /* Configure PA0 pin as input floating */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);

//  /* Enable SYSCFG clock */
//  
//  /* Connect EXTI0 Line to PA0 pin */
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource10);
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource12);

//  /* Configure EXTI0 line */
//  EXTI_InitStructure.EXTI_Line = EXTI_Line10|EXTI_Line11|EXTI_Line12;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);

//  /* Enable and set EXTI0 Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//}

static void SPI1_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(SPIx_CLK, ENABLE);
	RCC_AHBPeriphClockCmd(SPIx_SCK_GPIO_CLK | SPIx_MISO_GPIO_CLK | SPIx_MOSI_GPIO_CLK , ENABLE);
	
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_SOURCE, SPIx_SCK_AF);
	GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);
	GPIO_PinAFConfig(SPIx_MISO_GPIO_PORT, SPIx_MISO_SOURCE, SPIx_MISO_AF);
	
	GPIO_InitStructure.GPIO_Mode                 = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType                = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd                 = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed                = GPIO_Speed_Level_3;

	GPIO_InitStructure.GPIO_Pin                  = SPIx_SCK_PIN | SPIx_MOSI_PIN | SPIx_MISO_PIN;
  GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

	SPI_I2S_DeInit(SPIx);
  SPI_InitStructure.SPI_Direction              = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize               = SPI_DATASIZE;
  SPI_InitStructure.SPI_CPOL                   = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA                   = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS                    = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler      = SPI_BaudRatePrescaler_64;
	SPI_InitStructure.SPI_FirstBit               = SPI_FirstBit_MSB;
  /* Initializes the SPI communication */
  SPI_InitStructure.SPI_Mode                   = SPI_Mode_Master;
	//SPI_I2S_DeInit(SPI1);
  SPI_Init(SPIx, &SPI_InitStructure);
  
  /* Configure the SPI interrupt priority */
  NVIC_InitStructure.NVIC_IRQChannel           = SPIx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority   = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd        = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void Max7219_Init(uint8_t quantity)
{
	for(int i = 1;i < quantity+1;i++)
	{
	  Write_Max7219(0x09,0x00,i);
	  Write_Max7219(0x0A,0x01,i);
	  Write_Max7219(0x0B,0x07,i);
	  Write_Max7219(0x0C,0x01,i);
	}
	clear();
}
void Max7219_2_Init(void)
{
	Write_Max7219_2(0x09,0xFF,1);
	Write_Max7219_2(0x0A,0x01,1);
	Write_Max7219_2(0x0B,0x07,1);
	Write_Max7219_2(0x0C,0x01,1);
	clear_2();
}
void Write_Max7219(unsigned char address,unsigned char data,uint8_t quantity)
{
	Buffer = (data << 8) + address ;
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	for(int i=4;i>quantity;i--)
	{
	  SPI_I2S_SendData16(SPI1,0x0000);
	  while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET)
	  {}
	}
	SPI_I2S_SendData16(SPI1,Buffer);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET)
	{}
	for(int i=1;i<quantity;i++)
	{
	  SPI_I2S_SendData16(SPI1,0x0000);
	  while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET)
	  {}
	}
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}
void Write_Max7219_2(unsigned char address,unsigned char data,uint8_t quantity)
{
	Buffer = (data << 8) + address ;
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	SPI_I2S_SendData16(SPI1,Buffer);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET)
	{}
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
}

void SPICS1_Config(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}
void SPICS2_Config(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
}
static void TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);

  /* GPIOA Configuration: TIM3 CH1 (PA6) and TIM3 CH2 (PA7) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 

  /* Connect TIM Channels to AF1 */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_1);
  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);   

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 47;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Output Compare Toggle Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
}
void Delay_ms(__IO uint32_t nTime_ms)
{ 
  TimingDelay_ms = nTime_ms;

  while(TimingDelay_ms != 0);
}

 void TimingDelay_ms_Decrement(void)
{
  if (TimingDelay_ms != 0x00)
  { 
    TimingDelay_ms--;
  }
}
uint8_t transferangle(uint8_t number , uint8_t bit)
{
	int a[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	for(int i=0;i<8;i++)
	trBuffer[i] = 0x00;
	for(int i = 0;i<8;i++)
	{
		trBuffer[bit] += (num[number][7-i]&a[bit])>> (7-bit) ;
		if(i==7)
			break;
		trBuffer[bit] = trBuffer[bit] << 1;
	}
	return trBuffer[bit];
}
void GPIO_SCAN_Config(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}
//void keyscan(void)
//{	
//	switch(btnflag)
//	{
//		case 10:
//			EXTI_InitStructure.EXTI_LineCmd = DISABLE;
//			btnflag = 0;
//			scan(10);
//		  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//			break;
//		case 11:
//			EXTI_InitStructure.EXTI_LineCmd = DISABLE;
//		  btnflag = 0;
//			scan(11);
//		  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//			break;
//		case 12:
//			EXTI_InitStructure.EXTI_LineCmd = DISABLE;
//			btnflag =0;
//			scan(12);
//		  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//		  break;
//		default:
//			btnflag =0;
//			break;
//	}
//}
//void scan(uint8_t line)
//{
//	uint8_t s;
//	switch(line)
//	{
//		case 10:
//			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
//		  GPIO_SetBits(GPIOB,GPIO_Pin_14|GPIO_Pin_15);
//		  if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)==RESET)
//			{
//				Write_Max7219_2(0x01,x1,1);
//				x1++;
//				GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
//				break;
//			}
//			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
//		  GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_15);
//			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)==RESET)
//			{
//				Write_Max7219_2(0x02,x2,1);
//				x2++;
//				GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
//				break;
//			}
//			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
//		  GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14);
//			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)==RESET)
//			{
//				Write_Max7219_2(0x03,x3,1);
//				x3++;
//				GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
//				break;
//			}
//		case 11:
//			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
//		  GPIO_SetBits(GPIOB,GPIO_Pin_14|GPIO_Pin_15);
//		  if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)==RESET)
//			{
//				Write_Max7219_2(0x04,x4,1);
//				x4++;
//				GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
//				break; 
//			}
//			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
//		  GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_15);
//			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)==RESET)
//			{
//				Write_Max7219_2(0x05,x5,1);
//				x5++;
//				GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
//				break;
//			}
//			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
//		  GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14);
//			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)==RESET)
//			{
//				Write_Max7219_2(0x06,x6,1);
//				x6++;
//				GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
//				break;
//			}
//		case 12:
//			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
//		  GPIO_SetBits(GPIOB,GPIO_Pin_14|GPIO_Pin_15);
//		  if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)==RESET)
//			{
//				Write_Max7219_2(0x07,x7,1);
//				x7++;
//				GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
//				break;
//			}
//			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
//		  GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_15);
//			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)==RESET)
//			{
//				Write_Max7219_2(0x08,x8,1);
//				x8++;
//				GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
//				break;
//			}
//			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
//		  GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14);
//			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)==RESET)
//			{
//				exflag = 1;
//				GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
//				break;
//			}
//	}
//}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

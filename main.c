/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "mb.h"
#include "stm32f10x_usart.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START   1000
#define REG_INPUT_NREGS   4
#define REG_HOLDING_START 1000
#define REG_HOLDING_NREGS 130


/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];
static USHORT temp;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimingDelay;
GPIO_InitTypeDef GPIO_InitStructure;

ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void Delay(__IO uint32_t );
void USART1Config(void);
void GPIOConfig(void);
void Usart1Pass(void);
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  
  /*System set HSE (exteral high speed)*/
  // RCC_Configuration() ;
  // no need to call RCC_Configuration,as #define SYSCLK_FREQ_HSE    HSE_VALUE and HSE_VALUE=8M. 
  
  /*Config IO*/
  GPIOConfig();
  
  /*Set the SysTick IRQ period as 1ms*/
//  if (SysTick_Config(SystemCoreClock / 1000))
//  { 
//    /* Capture error */ 
//    while (1);
//  }
//  
    SystemInit();
  

  /*Inital the Uart1*/
   USART1Config();

   
  /* Retarget the C library printf function to the USARTx*/
   printf("\n\rUSART1 IS READY!\n\r");
   eMBInit(MB_RTU, 0x01, 1, 115200, MB_PAR_NONE); 
 
   eMBEnable(); 
  

   while (1)
  {
//    GPIO_WriteBit( GPIOC, GPIO_Pin_6, Bit_SET); 
//    GPIO_WriteBit( GPIOC, GPIO_Pin_7, Bit_SET);   
//    GPIO_WriteBit( GPIOC, GPIO_Pin_8, Bit_SET); 
//    GPIO_WriteBit( GPIOC, GPIO_Pin_9, Bit_SET); 
//    Delay((unsigned int )500);
//    i++;
////    printf("%d\n",i );
//    GPIO_WriteBit( GPIOC, GPIO_Pin_6, Bit_RESET); 
//    GPIO_WriteBit( GPIOC, GPIO_Pin_7, Bit_RESET);   
//    GPIO_WriteBit( GPIOC, GPIO_Pin_8, Bit_RESET); 
//    GPIO_WriteBit( GPIOC, GPIO_Pin_9, Bit_RESET); 
//    Delay((unsigned int )500);
//    i++;
////    printf("%d\n",i );
//    if(i==65535)
//      i=0;
//    
//    

 
	 eMBPoll();								   //不断的解析协议栈
 
        printf(" eMBPoll();");  
    
    
  }
//  for(;;);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}




/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : 开启gpioA B C，和usart1，2的时钟
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 	
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }

  }
}

/*******************************************************************************
* Function Name  : void USART1Config(void)
* Description    : 开启gpioA 和usart1的时钟,初始化配置波特率为115200
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1Config(void)
{
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
   /*note:GPIOA's is also need opened,so is USART1 */
  USART_InitTypeDef USART_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA
                           |RCC_APB2Periph_USART1, ENABLE);  
 /*USART1 confgure*/
  /* Configure USARTx_Tx as alternate function push-pull   PA9*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USARTx_Rx as input floating   PA10*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Configure the USARTx */ 
  USART_Init(USART1, &USART_InitStructure);
  /* Enable the USARTx */
  USART_Cmd(USART1, ENABLE); 
}

void GPIOConfig(void)
{
   /* GPIOC Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7| GPIO_Pin_8| GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

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

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


void Usart1Pass(void)
{
printf("\n\rUSART1 Pass!\n\r");
}  


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

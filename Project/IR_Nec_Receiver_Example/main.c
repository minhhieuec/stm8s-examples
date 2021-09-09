/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
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
#include "stm8s.h"
#include "stdio.h"
#include "stm8s_conf.h"

/**
  * @addtogroup UART1_Printf
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef _RAISONANCE_
#define PUTCHAR_PROTOTYPE int putchar (char c)
#define GETCHAR_PROTOTYPE int getchar (void)
#elif defined (_COSMIC_)
#define PUTCHAR_PROTOTYPE char putchar (char c)
#define GETCHAR_PROTOTYPE char getchar (void)
#else /* _IAR_ */
#define PUTCHAR_PROTOTYPE int putchar (int c)
#define GETCHAR_PROTOTYPE int getchar (void)
#endif /* _RAISONANCE_ */

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint16_t count_ms=0;
void CLK_Config(void);
void TIM4_Config(void);
void Delay(long x);

extern  uint32_t bitPattern;
char check_press=0,press=0;
char status=0;
void Dec2hex(unsigned long dec);
unsigned long dec2hex[8];

#define IR_CMD_POWER		0x00FFA25D
#define LED_OUT_PORT   GPIOC
#define LED_OUT_PIN    GPIO_PIN_3

void uart_debug_init(void)
{
	UART1_DeInit();
	/* UART1 configuration ------------------------------------------------------*/
	/* UART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - UART1 Clock disabled
  */
	UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
			   UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);

	/* Output a message on Hyperterminal using printf function */
	printf("\n\rUART1 Example :retarget the C library printf()/getchar() functions to the UART\n\r");
	printf("\n\rEnter Text\n\r");
}

void ir_cmd_recv_process(uint32_t val)
{
	uint16_t ir_cmd_high, ir_cmd_low;

  ir_cmd_high = (uint16_t)(val >> 16);
  ir_cmd_low = val;
  printf("ir: %x %x\n", ir_cmd_high, ir_cmd_low);

	if (val == IR_CMD_POWER)
	{
		printf("ctrl led \n");
		GPIO_WriteReverse(LED_OUT_PORT, LED_OUT_PIN);
	}
}

void main(void)
{
  CLK_Config();
  
  uart_debug_init();

  GPIO_Init(LED_OUT_PORT, LED_OUT_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteLow(LED_OUT_PORT, LED_OUT_PIN);

  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_FALL_ONLY);
  EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_FALL_ONLY);
  /////////////////////////////////////////
  TIM4_Config();
  enableInterrupts();
  /* Infinite loop */
  while (1)
  {

  }
  
}
/////////////////////////////#######################
void Dec2hex(unsigned long dec)
{
  for(uint8_t i=0;i<8;i++)
  {
    dec2hex[i]=dec;
    for(uint8_t j=0;j<i;j++)
    {
      dec2hex[i]=dec2hex[i]/16;
    }
    //
    dec2hex[i]=dec2hex[i]%16;
    if(dec2hex[i]>9)
    {
      dec2hex[i]=dec2hex[i]+55;
    }
    else dec2hex[i]=dec2hex[i]+48;
    ///
  }
}
//////////////////////////#######################
void CLK_Config(void)
{
  CLK_DeInit();
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      //f_Master = HSI/1 = 16MHz
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);            //f_CPU = f_Master/1 = 16MHz
  while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)!=SET);     //wait until HSI ready;
    
  
}
////////////////////////###########################################
void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 61);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  
  /* enable interrupts */
//  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}
//////////////////////////////////
void Delay(long x)
{
  while(x--)
  {
    __no_operation();
  }
}

/**
  * @brief Retargets the C library printf function to the UART.
  * @param c Character to send
  * @retval char Character sent
  */
PUTCHAR_PROTOTYPE
{
  /* Write a character to the UART1 */
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);

  return (c);
}

/////////////#####################################
#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

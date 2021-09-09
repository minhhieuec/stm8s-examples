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
/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint16_t prescaler = 15;
uint16_t period = 999;

void SystemClock_Init(void)
{
   CLK_DeInit();
   CLK_HSICmd(ENABLE);
   CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
   CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
}

void TIM1_Config_PWM(void)
{
  // TIM1 initialization
  TIM1_TimeBaseInit(prescaler, TIM1_COUNTERMODE_UP, period, 0);
  
  // Config do PWM1 - Duty Cycle = 50%
  TIM1_OC4Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE, ((period+1)/2), TIM1_OCPOLARITY_HIGH, TIM1_OCIDLESTATE_RESET);
  TIM1_OC4PreloadConfig(ENABLE);
  
  // Preload automatic reloading recorder
  TIM1_ARRPreloadConfig(ENABLE);
  
  // Activate peripheral TIM1
  TIM1_Cmd(ENABLE);
  
  // Activate TIM1 security
  TIM1_CtrlPWMOutputs(DISABLE);
}

void main()
{
	SystemClock_Init();

	GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST); // Output push pull, low level, 10MHz
	TIM1_Config_PWM();
	enableInterrupts();
	TIM1_CtrlPWMOutputs(ENABLE);

	while (1)
	{
		/* code */
	}
	
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
void assert_failed(u8 *file, u32 line)
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

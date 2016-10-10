/* Includes */
#include <stddef.h>
#include "stm32l1xx.h"


/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
uint32_t AD_value;
uint8_t step = 0;

/* Private function prototypes */

/* Private functions */
void adc_init(void);
void timer3_init(uint16_t period_ms);
void timer2_init(void);


int main(void)
{
	/* Initialize all configured peripherals */
	adc_init();
	timer2_init();
	timer3_init(100);

	/* Start ADC Software Conversion */
	ADC_SoftwareStartConv(ADC1);

  while(1) asm ("nop");

  return 0;
}


void adc_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/*Configure GPIO pin : PA5 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure ADCx Channel 2 as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Enable the HSI oscillator */
	RCC_HSICmd(ENABLE);

	/* Check that HSI oscillator is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

	/* Enable ADC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);

	/* ADC1 configuration */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADCx regular channel8 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_16Cycles);

	/* Enable the ADC */
	ADC_Cmd(ADC1, ENABLE);

	/* Wait until the ADC1 is ready */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);

}

/* TIM3 init function */
void timer3_init(uint16_t period_ms)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_Cmd(TIM3, DISABLE);
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 15999;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = period_ms-1;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3, &timerInitStructure);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM3_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);	
}

/* TIM2 init function */
void timer2_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_Cmd(TIM2, DISABLE);
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 15999;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 1;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 0;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

/* TIM2 Interrupt handler */
void TIM2_IRQHandler()
{
	ADC_SoftwareStartConv(ADC1);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) asm ("nop");
	AD_value=ADC_GetConversionValue(ADC1);

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		if (AD_value >= 1 && AD_value < 2 && step!=1)
		{
			timer3_init(10000);	// edit
			step = 1;
		}
		else if(AD_value >= 2 && AD_value < 3 && step!=2)
		{
			timer3_init(20000);	// edit
			step = 2;
		}
		else if(AD_value >= 3 && AD_value < 4 && step!=3)
		{
			timer3_init(30000);	// edit
			step = 3;
		}
		else if(AD_value >= 4 && AD_value < 5 && step!=4)
		{
			timer3_init(40000);	// edit
			step = 4;
		}
		else if(AD_value >= 5 && AD_value < 6 && step!=5)
		{
			timer3_init(50000);	// edit
			step = 5;
		}

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

/* TIM3 Interrupt handler */
void TIM3_IRQHandler()
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
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

/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
  while(1)
  {}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr)
{
   __assert_func (file, line, NULL, failedexpr);
}

#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f7xx.h"

#define BASIC_TIM           		TIM6
#define BASIC_TIM_CLK_ENABLE()     	__TIM6_CLK_ENABLE()

#define BASIC_TIM_IRQn				TIM6_DAC_IRQn
#define BASIC_TIM_IRQHandler    	TIM6_DAC_IRQHandler

extern TIM_HandleTypeDef TIM_Base;
void TIM_Basic_Init(void);

#endif /* __BASIC_TIM_H */


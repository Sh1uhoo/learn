#include "stm32f10x.h"                  // Device header

void Encoder_Init(void)
{

}

int16_t Encoder_Get(void)
{
	int16_t Temp;
	Temp = TIM_GetCounter(TIM4);
	return Temp;
}
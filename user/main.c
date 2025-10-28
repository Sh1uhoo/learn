#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include "Motor.h"
#include "Serial.h"
#include "Key.h"

float Target,Actual[2],Out[2];
float kp[2]={0.3,0.075},ki[2]={0.1,0.01},kd[2]={0.05,0.001};
float err0[2],err1[2],errint[2];
uint8_t State=0;
static int8_t Speed=0;

int main(void)
{
	OLED_Init();
	Key_Init();
	Motor_Init();
	Serial_Init();
	
	
	while(1)
	{
		
		
		State = Key_GetNum();
			
		if (Serial_GetRxFlag() == 1)
		{
			OLED_Clear();
			Speed = 0;
			int8_t k=1;
			
			uint8_t p=0;
			
			if (Serial_RxData[0] == '-') 
			{
				p++;
				k=-1;
			}
			
			while (Serial_RxData[p] != '\0')
			{
				Speed=Speed*10+(Serial_RxData[p++]-'0');
			}
			
			Speed*=k;
			
			
			
			
			OLED_ShowSignedNum(1,4,Speed,3);
			
			Serial_RxFlag = 0;
		}
		
		
		
		OLED_ShowSignedNum(2,4,State,3);
		
	}
	
}

void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Target = Speed;

		Actual[0] = Motor1_Get();
		Actual[1] = Motor2_Get();
		

		for (int i=0;i<2;i++)
		{
			err1[i] = err0[i];
			err0[i] = Target - Actual[i];
			errint[i] += err0[i];

			Out[i] = kp[State]*err0[i] + ki[State]*errint[i] + kd[State] * (err0[i]-err1[i]);

			if (Out[i] > 100 ) Out[i] = 100;
			if (Out[i] < -100 ) Out[i] = -100;
			if ((Actual[i] == 0 && Target == 0) || (Actual[i]-Target<=3 && Actual[i]-Target>=-3 && State == 1))
			{
				Out[i] = 0;
				errint[i] = 0;
			}
			else if (err1[i] == err0[i] && State == 1) Out[i] *= 2.2;

			Motor_Setspeed(Out[i],i);
		}

		printf("%f,%f,%f,%f,%f\n",Actual[0],Actual[1],Out[0],Out[1],Target);

		TIM_ClearITPendingBit(TIM2 , TIM_IT_Update);
	}
}

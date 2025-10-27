#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include "Motor.h"
#include "Serial.h"
#include "Key.h"

float Target,Actual1,Actual2,Actual,Out;
float kp[2]={0.3,0.075},ki[2]={0.1,0.01},kd[2]={0.05,0.001};
float err0,err1,errint;
uint8_t State=0;
static int8_t Speed=0;
static int8_t loc=0;

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
		if (State == 0)
				Target = Speed;
		
		
		Actual2 = Motor2_Get();
		Actual1 = Motor1_Get();
		Actual = (Actual1 + Actual2)/2.0;
		
		
		err1=err0;
		err0=Target - Actual;
		errint += err0;
		
		Out = kp[State]*err0 + ki[State]*errint + kd[State] * (err0-err1);
		
		if (Out > 100 ) Out = 100;
		if (Out < -100 ) Out = -100;
		if ((Actual == 0 && Target == 0) || (Actual-Target<=3 && Actual-Target>=-3 && State == 1))
		{
			Out = 0;
			errint = 0;
		}
		else if (err1 == err0 && State == 1) Out*=2.2;

		Motor_Setspeed(Out);

		printf("%f,%f,%f,%f,%f\n",Actual1,Actual2,Actual,Out,Target);

		TIM_ClearITPendingBit(TIM2 , TIM_IT_Update);
	}
}

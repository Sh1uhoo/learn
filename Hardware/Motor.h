#ifndef __Motor_H
#define __Motor_H

void Motor_Init(void);
void Motor_Setspeed(int8_t Speed,uint8_t MotorNum);
 int16_t Motor1_Get(void);
 int16_t Motor2_Get(void);

#endif

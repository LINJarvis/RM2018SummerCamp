#ifndef __EXECUTE_TASK_H__
#define __EXECUTE_TASK_H__

#include "stm32f4xx_hal.h"

void execute_task(const void* argu);

void arm_moto_init(void);
void arm_moto_control(void);

void storage_moto_init(void);
void storage_moto_control(void);

void io_pwm_control(void);
//void Usart3_callback(void);

#endif


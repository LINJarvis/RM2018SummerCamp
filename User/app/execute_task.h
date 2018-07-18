#ifndef __EXECUTE_TASK_H__
#define __EXECUTE_TASK_H__

#include "stm32f4xx_hal.h"
#include "rm_hal_lib.h"

void execute_task(const void* argu);

void test_moto_init(void);
void test_moto_control(void);

extern imu_t imu1;

#endif


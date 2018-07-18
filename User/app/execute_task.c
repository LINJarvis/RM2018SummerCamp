#include "execute_task.h"
#include "can_device.h"
#include "uart_device.h"
#include "cmsis_os.h"
#include "calibrate.h"
#include "pid.h"
#include "sys.h"



//uint8_t test_key_value;
extern uint8_t test_servo;
imu_t imu1;

//uint16_t servo_angle = 1000;



void execute_task(const void* argu)
{
   //测试电机初始化	
	
   while(1)
  {
   //测试电机控制程序
		get_imu_data(&imu1);
    osDelay(50);
  }
}

#include "execute_task.h"
#include "can_device.h"
#include "uart_device.h"
#include "cmsis_os.h"
#include "calibrate.h"
#include "pid.h"
#include "sys.h"

//uint8_t test_key_value;
extern uint8_t test_servo;
extern uint8_t if_pick;
extern uint8_t if_put;
extern uint8_t which_storage; // 0 is left, 1 is right
long pick_time_begin = 0;
//uint16_t servo_angle = 1000;

void execute_task(const void *argu)
{
    //测试电机初始化
    arm_moto_init();
    storage_moto_init();

    while (1)
    {
        if (rc.sw2 == 3)
        {
            test_servo = 1;
        }

        else
        {
            test_servo = 0;
        }

        if (rc.kb.bit.X)
        {
            if (rc.kb.bit.SHIFT == 0)
            {
                if_pick = 1;
                pick_time_begin = HAL_GetTick();
            }
            else
            {
                if_put = 1;
                pick_time_begin = HAL_GetTick();
            }
        }
        if (rc.kb.bit.V)
        {
            if (rc.kb.bit.SHIFT == 0)
            {
                which_storage = 0;
            }
            else
            {
                which_storage = 1;
            }
        }
        if (rc.kb.bit.B)
        {
            if (rc.kb.bit.SHIFT == 0)
            {
                which_storage = 0;
            }
            else
            {
                which_storage = 2;
            }
        }

        arm_moto_control();
        storage_moto_control();

        osDelay(10);
    }
}

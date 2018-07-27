#include "execute_task.h"
#include "can_device.h"
#include "uart_device.h"
#include "cmsis_os.h"
#include "calibrate.h"
#include "pid.h"
#include "sys.h"

//uint8_t test_key_value;
extern uint8_t relay;
extern uint8_t if_pick;
extern uint8_t if_put;
extern uint8_t which_storage; // 0 is left, 1 is right
extern uint8_t usart3_recv[];
long pick_time_begin = 0;
//uint16_t servo_angle = 1000;

void execute_task(const void *argu)
{
    //测试电机初始化
    arm_moto_init();
    storage_moto_init();
	  uart_init(USER_UART3,9600,WORD_LEN_8B,STOP_BITS_1,PARITY_NONE);
	  //uart_recv_callback_register(USER_UART3,Usart3_callback);
		uart_receive_start(USER_UART3,usart3_recv,4);

    while (1)
    {
        if ((rc.kb.bit.X && rc.kb.bit.SHIFT == 0)|| rc.sw1 == 1)
        {
            if_pick = 1;
            pick_time_begin = HAL_GetTick();
        }
        if ((rc.kb.bit.X && rc.kb.bit.SHIFT == 1) || rc.sw1 == 2)
        {
            if_put = 1;
            pick_time_begin = HAL_GetTick();
        }

        if (rc.kb.bit.Q)
        {
            which_storage = 1;
        }
        if (rc.kb.bit.E)
        {
            which_storage = 2;
        }

        arm_moto_control();
        storage_moto_control();
        io_pwm_control();

				
        osDelay(7);
    }
}

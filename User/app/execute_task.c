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
extern uint8_t turn_over;
extern uint8_t usart3_recv[];
extern int CV_offest;

extern int16_t arms[3][4];

long pick_time_begin = 0;
long turn_time_begin = 0;
//uint16_t servo_angle = 1000;

uint8_t stop = 0;
uint8_t mode = 0;

void Usart3_callback()
{
	if (usart3_recv[0] == '-')
		CV_offest = -(usart3_recv[1] * 100 + usart3_recv[2] * 10 + usart3_recv[3]);
	else if (usart3_recv[0] == '0')
	{
		CV_offest = usart3_recv[1] * 100 + usart3_recv[2] * 10 + usart3_recv[3];
	}
}

void execute_task(const void *argu)
{
	//测试电机初始化
	arm_moto_init();
	storage_moto_init();
	/*
    uart_init(USER_UART3, 9600, WORD_LEN_8B, STOP_BITS_1, PARITY_NONE);
    uart_recv_callback_register(USER_UART3, Usart3_callback);
    uart_receive_start(USER_UART3, usart3_recv, 4);
*/
	write_led_io(LASER_IO, LED_ON);

	while (1)
	{
		if ((rc.kb.bit.G && rc.kb.bit.CTRL == 1))
		{
			stop = 1;
		}
		if (rc.kb.bit.G || rc.sw2 == 3)
		{
			stop = 0;
		}
		if (stop == 0)
		{
			if (rc.kb.bit.X && rc.kb.bit.SHIFT == 1)
			{
				mode = 0;
			}
			if (rc.kb.bit.C && rc.kb.bit.SHIFT == 1)
			{
				mode = 1;
			}

			if (mode == 1)
			{
				if (rc.mouse.l || rc.sw1 == 1)
				{
					if_pick = 2;
					pick_time_begin = HAL_GetTick();
				}
				if (rc.mouse.r || rc.sw1 == 2)
				{
					if_put = 2;
					pick_time_begin = HAL_GetTick();
				}
			}
			if (mode == 0)
			{
				if (rc.mouse.l || rc.sw1 == 1)
				{
					if_pick = 1;
					pick_time_begin = HAL_GetTick();
				}
				if (rc.mouse.r || rc.sw1 == 2)
				{
					if_put = 1;
					pick_time_begin = HAL_GetTick();
				}
			}
			if (rc.kb.bit.Q)
			{
				which_storage = 1;
			}
			if (rc.kb.bit.E)
			{
				which_storage = 2;
			}

			if ((rc.kb.bit.SHIFT == 1) && rc.kb.bit.R)
			{
				turn_over = 1;
				turn_time_begin = HAL_GetTick();
			}

			arm_moto_control();
			//storage_moto_control();
			io_pwm_control();

			osDelay(7);
		}
		if (stop == 1)
		{
			for (int i = 0; i < 4; i++)
			{
				arms[2][i] = 0;
			}
			moto_arms[3].total_angle = 0;
		}
	}
}

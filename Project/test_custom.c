#include "execute_task.h"
#include "can_device.h"
#include "uart_device.h"
#include "pid.h"
#include "sys.h"

int16_t test_moto_speed = 0;
int16_t test_moto_current[1];

uint8_t test_servo = 0;
uint8_t if_pick = 0;
uint8_t if_put = 0;
uint8_t which_storage = 0; // 0 is left, 1 is right
extern long pick_time_begin;

uint8_t ir_sensor[6];

int16_t arms[3][4];
// CAN1: 0x1ff
//   [X] [0] ecd_set [1] speed_set [2] current
//       [0] 5 - fric wheel 1 (2006)
//       [1] 6 - fric wheel 2 (2006)
//       [2] 7 - up&down      (3508)
//       [3] 8 - pitch        (3508)

int16_t storage[3][2];
// CAN2: 0x200
//   [X] [0] ecd_set [1] speed_set [2] current
//       [0] 1 - somewhat (2006)
//       [1] 2 - somewhat (2006)

int16_t height;

void arm_moto_control(void)
{
    //舵机控制函数周期设定
    //set_pwm_group_param(PWM_GROUhP1, 20000);

    //开启控制端口
    //start_pwm_output(PWM_IO1);
    //set_pwm_param(PWM_IO1,2200);

    //relay trigger
    int someecd = 0;
    int time_after_start = HAL_GetTick() - pick_time_begin;

    if (if_pick == 1)
    {
        height = 0; // reset no. of blocks

        read_digital_io(5, &ir_sensor[0]);  //block 0
        read_digital_io(6, &ir_sensor[1]);  //block 1
        read_digital_io(7, &ir_sensor[2]);  //block 2
        read_digital_io(8, &ir_sensor[3]);  //block 3
        read_digital_io(9, &ir_sensor[4]);  //block 4
        read_digital_io(10, &ir_sensor[5]); //block 5

        for (int i = 0; i <= 3; i++) // get no. of blocks
        {
            if (ir_sensor[i] == 1)
                height++;
        }

        if (time_after_start < 0.3 * 20)
        {
            arms[0][3] = someecd; //pitch goes downward
            arms[0][0] = someecd; //fric goes
            arms[0][1] = someecd;
        }
        if (time_after_start > 0.3 * 20 && time_after_start < 0.5 * 20)
        {
            arms[0][2] = someecd; //up&down goes upward
                                  // write_digital_io(1, 1);
        }
        if (time_after_start > 0.5 * 20 && time_after_start < 0.7 * 20)
            arms[0][3] = someecd; //pitch goes upward, block picked
        if (time_after_start > 0.7 * 20 && time_after_start < 1 * 20)
        {
            arms[0][3] = someecd; //pitch goes upward, block picked
            arms[0][0] = someecd; //fric goes
            arms[0][1] = someecd;
        }
        if (time_after_start > 1 * 20 && time_after_start > 1.5 * 20) // finished picking
        {
            arms[0][2] = someecd; //up&down goes downward
            arms[0][3] = someecd; //pitch goes downward but little lifted up
            arms[0][0] = someecd; //fric goes back
            arms[0][1] = someecd;
        }
        if_pick = 0;
    }
    else
    {
        arms[0][2] = someecd; //up&down goes downward
        arms[0][3] = someecd; //pitch goes downward but little lifted up
        arms[0][0] = someecd; //fric goes back
        arms[0][1] = someecd;
    }

    // calculate and send currents to motors
    arms[2][0] = pid_calc(&pid_arms[1][0], moto_arms[0].speed_rpm, pid_calc(&pid_arms[0][0], moto_arms[0].ecd, arms[0][0]));
    arms[2][1] = pid_calc(&pid_arms[1][1], moto_arms[1].speed_rpm, pid_calc(&pid_arms[0][1], moto_arms[1].ecd, arms[0][1]));
    arms[2][2] = pid_calc(&pid_arms[1][2], moto_arms[2].speed_rpm, pid_calc(&pid_arms[0][2], moto_arms[2].ecd, arms[0][2]));
    arms[2][3] = pid_calc(&pid_arms[1][3], moto_arms[3].speed_rpm, pid_calc(&pid_arms[0][3], moto_arms[3].ecd, arms[0][3]));

    storage[2][0] = pid_calc(&pid_storage[1][0], moto_storage[0].speed_rpm, pid_calc(&pid_storage[0][0], moto_storage[0].ecd, storage[0][0]));
    storage[2][1] = pid_calc(&pid_storage[1][1], moto_storage[1].speed_rpm, pid_calc(&pid_storage[0][1], moto_storage[1].ecd, storage[0][1]));
    send_arm_moto_current(arms[2]);
    send_storage_moto_current(storage[2]);

    //  扩展板的按键控制电机
    //      read_key_io(KEY_IO1,&key1);
    //			if(key1 ==1)
    //		{
    //			 set_test_motor_current(test_moto_current);
    //			}
}
//电机初始化参数设定
void arm_moto_init(void)
{
    // PID init:
    //   [0] ecd_set
    //       [0] 5 - fric wheel 1 (2006)
    //       [1] 6 - fric wheel 2 (2006)
    //       [2] 7 - up&down      (3508)
    //       [3] 8 - pitch        (3508)
    pid_init(&pid_arms[0][0], 7000, 0, 1, 0, 0);
    pid_init(&pid_arms[0][1], 7000, 0, 1, 0, 0);
    pid_init(&pid_arms[0][2], 7000, 0, 1, 0, 0);
    pid_init(&pid_arms[0][3], 7000, 0, 1, 0, 0);

    // PID init:
    //   [1] speed_set
    //       [0] 5 - fric wheel 1 (2006)
    //       [1] 6 - fric wheel 2 (2006)
    //       [2] 7 - up&down      (3508)
    //       [3] 8 - pitch        (3508)
    pid_init(&pid_arms[1][0], 7000, 0, 1, 0, 0);
    pid_init(&pid_arms[1][1], 7000, 0, 1, 0, 0);
    pid_init(&pid_arms[1][2], 7000, 0, 1, 0, 0);
    pid_init(&pid_arms[1][3], 7000, 0, 1, 0, 0);

    set_digital_io_dir(1, IO_OUTPUT);
    set_digital_io_dir(2, IO_INPUT);
}

void storage_moto_control(void)
{
    //relay trigger
    int someecd = 0;
    int time_after_start = HAL_GetTick() - pick_time_begin;

    if (which_storage == 0)
    {
        storage[0][0] = someecd;
    }
    else
    {
        storage[0][1] = someecd;
    }
    storage[2][0] = pid_calc(&pid_storage[1][0], moto_storage[0].speed_rpm, pid_calc(&pid_storage[0][0], moto_storage[0].ecd, storage[0][0]));
    //storage[2][1] = pid_calc(&pid_storage[1][1], moto_storage[1].speed_rpm, pid_calc(&pid_storage[0][1], moto_storage[1].ecd, storage[0][1]));
    send_storage_moto_current(storage[2]);
}

void storage_moto_init(void)
{
    // PID init:
    //   [0] ecd_set
    //       [0] 1 - somewhat (2006)
    //       [1] 2 - somewhat (2006)

    pid_init(&pid_storage[0][0], 7000, 0, 1, 0, 0);
    pid_init(&pid_storage[0][1], 7000, 0, 1, 0, 0);

    // PID init:
    //   [1] speed_set
    //       [0] 1 - somewhat (2006)
    //       [1] 2 - somewhat (2006)

    pid_init(&pid_storage[1][0], 7000, 0, 1, 0, 0);
    pid_init(&pid_storage[1][1], 7000, 0, 1, 0, 0);
}

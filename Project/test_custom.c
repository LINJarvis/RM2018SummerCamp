#include "execute_task.h"
#include "can_device.h"
#include "uart_device.h"
#include "pid.h"
#include "sys.h"

int16_t test_moto_speed = 0;
int16_t test_moto_current[1];

uint8_t relay = 0;
uint8_t if_pick = 0;
uint8_t if_put = 0;
uint8_t which_storage = 1; // 1 is left, 2 is right
extern long pick_time_begin;
int time_after_start;

uint8_t ir_sensor[6];

int16_t arms[3][4];
// CAN1: 0x1ff
//   [X] [0] angle_set [1] speed_set [2] current
//       [0] 5 - fric wheel 1 (2006)
//           init: 0, situ1: 60, situ2: N/A
//       [1] 6 - fric wheel 2 (2006)
//           any situ: - (fric wheel 1)
//       [2] 7 - up&down      (3508)
//           init: N/A, situ1: N/A, situ2: -500
//       [3] 8 - pitch        (3508)
//           init: 60, situ1: 0, situ2: 270
//           max: 1330

int16_t storage[3][2];
// CAN2: 0x200
//   [X] [0] angle_set [1] speed_set [2] current
//       [0] 1 - l/r (2006)
//           l: -100(init), med: 500, r: 1100
//       [1] 2 - somewhat (2006)

int16_t height;

void io_pwm_control(void)
{
	
    if (relay == 1)
        write_digital_io(8, 1);
    else

        write_digital_io(8, 0);
}

void arm_moto_control(void)
{
    //舵机控制函数周期设定
    //set_pwm_group_param(PWM_GROUP1, 20000);

    //开启控制端口
    //start_pwm_output(PWM_IO1);
    //set_pwm_param(PWM_IO1,2200);

    time_after_start = (HAL_GetTick() - pick_time_begin) / 100;

    // auto pick
    if (if_pick == 1)
    {
        pid_init(&pid_arms[0][3], 7000, 0, 50, 0.1, 0.1);
        height = 0; // reset no. of blocks

        read_digital_io(5, &ir_sensor[0]);  //block 0
        read_digital_io(6, &ir_sensor[1]);  //block 1
        read_digital_io(7, &ir_sensor[2]);  //block 2
        read_digital_io(8, &ir_sensor[3]);  //block 3
        read_digital_io(9, &ir_sensor[4]);  //block 4
        read_digital_io(10, &ir_sensor[5]); //block 5

        for (int i = 0; i < 6; i++) // get no. of blocks
        {
            if (ir_sensor[i] == 1)
                height++;
        }

        if (time_after_start < 5)
        {
            if (which_storage == 1)
                storage[0][0] = -50;
            if (which_storage == 2)
                storage[0][0] = 1200;
            arms[0][3] = 0; // pitch goes downward
        }

        if (time_after_start >= 5 && time_after_start < 10)
        {
            arms[0][0] = 60; // fric goes, brick picked
            arms[0][1] = -60;
        }

        if (time_after_start >= 10 && time_after_start < 20)
            arms[0][2] = 400; // up&down goes upward to value set

        if (time_after_start >= 20 && time_after_start < 40)
        {
            pid_init(&pid_arms[0][3], 7000, 0, 50, 0.1, 0.1);
            arms[0][3] = 180; // pitch goes upward to critical
        }

        if (time_after_start >= 40 && time_after_start < 45)
        {
            pid_init(&pid_arms[0][3], 7000, 0, 0.4, 0, 0);
            arms[0][3] = 230; // pitch goes from critical to value set
        }

        if (time_after_start >= 45 && time_after_start < 47)
        {
            arms[0][0] = 160; // fric continues going, brick stored
            arms[0][1] = -160;
        }

        if (time_after_start >= 47 && time_after_start < 60)
        {
            pid_init(&pid_arms[0][3], 7000, 0, 50, 0.1, 0.1);
            arms[0][3] = 100; // pitch goes back to critical vaule
        }

        if (time_after_start >= 60 && time_after_start < 65)
        {
            pid_init(&pid_arms[0][3], 7000, 0, -1, 0, 0);
            arms[0][3] = 70; // pitch resets
        }
        if (time_after_start >= 65)
        {
            storage[0][0] = 500;
            arms[0][2] = 0; // up&down resets
            arms[0][0] = 0; // fric goes back
            arms[0][1] = 0;
            if_pick = 0;
        }
    }

    if (if_put == 1)
    {
        height = 0; // reset no. of blocks

        read_digital_io(3, &ir_sensor[0]); //block 0
        read_digital_io(4, &ir_sensor[1]); //block 1
        read_digital_io(5, &ir_sensor[2]); //block 2
        read_digital_io(6, &ir_sensor[3]); //block 3
        read_digital_io(7, &ir_sensor[4]); //block 4
        read_digital_io(8, &ir_sensor[5]); //block 5

        for (int i = 0; i < 6; i++) // get no. of blocks
        {
            if (ir_sensor[i] == 1)
                height++;
        }

        if (time_after_start >= 10 && time_after_start < 20)
        {
            if (which_storage == 1)
                storage[0][0] = -50;
            if (which_storage == 2)
                storage[0][0] = 1200;
            arms[0][2] = 400; // up&down goes upward to value set
        }

        if (time_after_start >= 20 && time_after_start < 40)
        {
            pid_init(&pid_arms[0][3], 7000, 0, 50, 0.1, 0.1);
            arms[0][3] = 180; // pitch goes upward to critical
        }

        if (time_after_start >= 40 && time_after_start < 45)
        {
            arms[0][2] = 0; // up&down goes upward to value set
            pid_init(&pid_arms[0][3], 7000, 0, 1.5, 0, 0);
            arms[0][3] = 230; // pitch goes from critical to value set
        }

        if (time_after_start >= 45 && time_after_start < 57)
        {
            arms[0][0] = -60; // fric
            arms[0][1] = 60;
        }

        if (time_after_start >= 57 && time_after_start < 70)
        {
            arms[0][2] = 400; // up&down goes upward to value set
            pid_init(&pid_arms[0][3], 7000, 0, 40, 0.1, 0.1);
            arms[0][3] = 100; // pitch goes back to critical vaule
        }

        if (time_after_start >= 70 && time_after_start < 75)
        {
            pid_init(&pid_arms[0][3], 7000, 0, -0.5, 0, 0);
            arms[0][3] = 0; // pitch resets
        }
        if (time_after_start >= 75 && time_after_start < 80)
        {
            pid_init(&pid_arms[0][3], 7000, 0, -1, 0, 0);
            //arms[0][2] = 0;  // reset downward
        }

        if (time_after_start >= 80 && time_after_start < 85)
        {
            arms[0][0] = -140; // brick released
            arms[0][1] = 140;
        }

        if (time_after_start >= 85 && time_after_start < 90)
        {
            pid_init(&pid_arms[0][3], 7000, 0, 50, 0.1, 0.1);
            storage[0][0] = 500;
            arms[0][3] = 70; // pitch resets
            arms[0][2] = 0;  // up&down resets
            arms[0][0] = 0;  // fric goes back
            arms[0][1] = 0;
            if_put = 0;
        }
    }

    if (if_pick == 0 && if_put == 0)
    {

        pid_init(&pid_arms[0][3], 7000, 0, 50, 0.1, 0.1);
        arms[0][3] = 70; // pitch resets
        arms[0][2] = 0;  // up&down resets
        arms[0][0] = 0;  // fric goes back
        arms[0][1] = 0;
        //write_digital_io(8, 0);
    }

    // calculate and send currents to motors
    arms[2][0] = pid_calc(&pid_arms[1][0], moto_arms[0].speed_rpm,
                          pid_calc(&pid_arms[0][0], moto_arms[0].total_angle / 36.0, arms[0][0]));
    arms[2][1] = pid_calc(&pid_arms[1][1], moto_arms[1].speed_rpm,
                          pid_calc(&pid_arms[0][1], moto_arms[1].total_angle / 36.0, arms[0][1]));
    arms[2][2] = pid_calc(&pid_arms[1][2], moto_arms[2].speed_rpm,
                          pid_calc(&pid_arms[0][2], moto_arms[2].total_angle / 19.2, arms[0][2]));
    arms[2][3] = pid_calc(&pid_arms[1][3], moto_arms[3].speed_rpm,
                          pid_calc(&pid_arms[0][3], moto_arms[3].total_angle / 19.2, arms[0][3]));

    send_arm_moto_current(arms[2]);
}

void arm_moto_init(void)
{
    // PID init:
    //   [0] angle_set
    //       [0] 5 - fric wheel 1 (2006)
    //       [1] 6 - fric wheel 2 (2006)
    //       [2] 7 - up&down      (3508)
    //       [3] 8 - pitch        (3508)
    pid_init(&pid_arms[0][0], 7000, 0, 35, 0.1, 0);
    pid_init(&pid_arms[0][1], 7000, 0, 35, 0.1, 0);
    pid_init(&pid_arms[0][2], 7000, 0, 26, 0.1, 0);
    pid_init(&pid_arms[0][3], 7000, 0, 50, 0, 0);

    // PID init:
    //   [1] speed_set
    //       [0] 5 - fric wheel 1 (2006)
    //       [1] 6 - fric wheel 2 (2006)
    //       [2] 7 - up&down      (3508)
    //       [3] 8 - pitch        (3508)
    pid_init(&pid_arms[1][0], 7000, 0, 10, 0, 0);
    pid_init(&pid_arms[1][1], 7000, 0, 10, 0, 0);
    pid_init(&pid_arms[1][2], 7000, 0, 10, 0, 0);
    pid_init(&pid_arms[1][3], 10000, 0, 10, 0, 0);
}

void storage_moto_control(void)
{
    storage[2][0] = pid_calc(&pid_storage[1][0], moto_storage[0].speed_rpm,
                             pid_calc(&pid_storage[0][0], moto_storage[0].total_angle / 36.0, storage[0][0]));
    //storage[2][1] = pid_calc(&pid_storage[1][1], moto_storage[1].speed_rpm,
    //                       pid_calc(&pid_storage[0][1], moto_storage[1].total_angle / 36.0, storage[0][1]));

    send_storage_moto_current(storage[2]);
}

void storage_moto_init(void)
{
    // PID init:
    //   [0] angle_set
    //       [0] 1 - l/r (2006)
    //       [1] 2 - somewhat (2006)

    pid_init(&pid_storage[0][0], 10000, 0, 15, 0.1, 0.5);
    pid_init(&pid_storage[0][1], 7000, 0, 0.5, 0.1, 0);

    // PID init:
    //   [1] speed_set
    //       [0] 1 - l/r (2006)
    //       [1] 2 - somewhat (2006)

    pid_init(&pid_storage[1][0], 10000, 0, 2, 0.1, 0.5);
    pid_init(&pid_storage[1][1], 7000, 0, 1, 0.1, 0);

	
    set_digital_io_dir(1, IO_INPUT);
    set_digital_io_dir(2, IO_INPUT);
    set_digital_io_dir(3, IO_INPUT);
    set_digital_io_dir(4, IO_INPUT);
    set_digital_io_dir(5, IO_INPUT);
    set_digital_io_dir(6, IO_INPUT);
		
    set_digital_io_dir(8, IO_OUTPUT);

    storage[0][0] = 500;
}

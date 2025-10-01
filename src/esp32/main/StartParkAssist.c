/*
 * File Name: StartParkAssist.c
 * Author: Pham Tran Duc Hanh
 * Created on: Tue Sep 30 2025
 *
 */

#include "StartParkAssist.h"
#define TAG "CAN_TASK"

void app_main(void)
{
    can_init();

    xTaskCreate(can_handler, "can_task", 4096, NULL, 12, NULL);
    xTaskCreate(oled_display, "oled_task", 4096, NULL, 9, NULL);
    xTaskCreate(fsm_run, "running_fsm", 4096, NULL, 10, NULL);
}
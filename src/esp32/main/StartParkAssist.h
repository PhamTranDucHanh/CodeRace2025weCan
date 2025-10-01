/*
 * File Name: StartParkAssist.h
 * Author: Pham Tran Duc Hanh
 * Created on: Tue Sep 30 2025
 *
 */

#pragma once

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <ssd1306.h>
#include "driver/twai.h"
#include <inttypes.h>
#include "can_handler.h"
#include "OLED_display.h"
#include <string.h>
#include "fsm.h"

//#define Monitor_Debugging
/*
 * File Name: fsm.c
 * Author: Pham Tran Duc Hanh
 * Created on: Tue Sep 30 2025
 *
 */


#include "fsm.h"


CarState currentState = INIT;

void fsm_run()
{
    // Đăng ký task FSM với watchdog
    esp_task_wdt_add(NULL);

    while (1) {
        switch (currentState)
        {
            case INIT:
                if (getGear() == P && getpBrakeOn() && getEngineOn()) currentState = START_UP;
                if (getGear() == P && ! getpBrakeOn() && getEngineOn()) currentState = ENGAGE_P_BRAKE_S;
                if (getGear() != P && getpBrakeOn() && getEngineOn()) currentState = RELEASE_P_BRAKE;
                if (getGear() != P && ! getpBrakeOn() && getEngineOn()) currentState = DRIVING;
                if (getGear() != P && ! getpBrakeOn() && ! getEngineOn() && ! getBrakeOn()) currentState = ROLL_AWAY;
                if (getGear() != P && ! getpBrakeOn() && ! getEngineOn() && getBrakeOn()) currentState = PULL_OVER_SAFELY;
                if (getGear() == P && ! getpBrakeOn() && ! getEngineOn()) currentState = ENGAGE_P_BRAKE_P;
                if (getGear() != P && getpBrakeOn() && ! getEngineOn()) currentState = SHIFT_TO_P;
                if (getGear() == P && getpBrakeOn() && ! getEngineOn()) currentState = PARK_CORRECTLY;

                if (currentState == INIT) currentState = ERROR;
                break;
            case ERROR:
                display_state(ERROR);           // Hiển thị thông báo lỗi lên OLED
                vTaskDelay(pdMS_TO_TICKS(3000)); 
                esp_restart();                  // Reset lại ESP32
                break;
            case START_UP:
                display_state (START_UP);

                if (! getpBrakeOn()) currentState = ENGAGE_P_BRAKE_S;
                if (getGear() != P) currentState = RELEASE_P_BRAKE;
                if (! getEngineOn()) currentState = PARK_CORRECTLY;
                break;
            case ENGAGE_P_BRAKE_S:
                display_state (ENGAGE_P_BRAKE_S);

                if (getpBrakeOn()) currentState = START_UP;
                break;
            case RELEASE_P_BRAKE:
                display_state (RELEASE_P_BRAKE);

                if (! getpBrakeOn()) currentState = DRIVING;
                if (getGear() == P) currentState = START_UP;
                break;
            case PULL_OVER_SAFELY:
                display_state (PULL_OVER_SAFELY);

                if (getEngineOn()) currentState = DRIVING;
                break;
            case ROLL_AWAY:
                display_state (ROLL_AWAY);

                if (getEngineOn()) currentState = DRIVING;
                if (getBrakeOn()) currentState = PULL_OVER_SAFELY;
                if (getGear() == P) currentState = ENGAGE_P_BRAKE_P;
                if (getpBrakeOn()) currentState = SHIFT_TO_P;
                break;
            case ENGAGE_P_BRAKE_P:
                display_state (ENGAGE_P_BRAKE_P);

                if (getGear() != P) currentState = ROLL_AWAY;
                if (getpBrakeOn()) currentState = PARK_CORRECTLY;
                break;
            case SHIFT_TO_P:
                display_state (SHIFT_TO_P);

                if (! getpBrakeOn()) currentState = ROLL_AWAY;
                if (getGear() == P) currentState = PARK_CORRECTLY;
                break;
            case PARK_CORRECTLY:
                display_state (PARK_CORRECTLY);
                
                if (getEngineOn()) currentState = START_UP;
                if (! getpBrakeOn()) currentState = ENGAGE_P_BRAKE_P;
                if (getGear() != P) currentState = SHIFT_TO_P;
                break;
            case DRIVING:
                display_state (DRIVING);

                if (getpBrakeOn()) currentState = RELEASE_P_BRAKE;
                if (! getEngineOn()) currentState = ROLL_AWAY;
                break;
            default:
                currentState = INIT;
                break;
        }

        // Feed watchdog sau mỗi chu kỳ xử lý logic
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(10)); // Nghỉ ngắn để tránh chiếm CPU 100%
    }
}
/*
 * File Name: OLED_display.h
 * Author: Pham Tran Duc Hanh
 * Created on: Tue Sep 30 2025
 *
 */

#pragma once

#include "StartParkAssist.h"
#include "fsm.h"

typedef enum {
  INIT,
  ERROR,
  START_UP,
  ENGAGE_P_BRAKE_S,
  RELEASE_P_BRAKE,
  PULL_OVER_SAFELY,
  ROLL_AWAY,
  ENGAGE_P_BRAKE_P,
  SHIFT_TO_P,
  PARK_CORRECTLY,
  DRIVING
} CarState;

void oled_display();
void display_state(CarState state);
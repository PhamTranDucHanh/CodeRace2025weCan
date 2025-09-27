#pragma once

#include "StartParkAssist.h"
#include "fsm.h"

typedef enum {
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
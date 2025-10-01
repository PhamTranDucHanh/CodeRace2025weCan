/*
 * File Name: can_handler.h
 * Author: Pham Tran Duc Hanh
 * Created on: Tue Sep 30 2025
 *
 */

#pragma once
#include "StartParkAssist.h"

typedef enum{
    P,
    N,
    R,
    D,
    S
} Gear;

void can_init();
void can_handler();

Gear getGear();
bool getEngineOn();
bool getpBrakeOn();
bool getBrakeOn();

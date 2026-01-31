/*
 * File Name: OLED_display.c
 * Author: Pham Tran Duc Hanh
 * Created on: Tue Sep 30 2025
 *
 */

#include "OLED_display.h"
void display_infor();
char line1_1[64] = "";
char line1_2[64] = "";
char line2_1[64] = "";
char line2_2[64] = "" ;

char line3[64] = "";
char line4[64] = "";
char line5[64] = "";

void oled_display(){
    init_ssd1306();

    while (1) {
        clear_ssd1306();
        display_infor();
        ssd1306_print_str(4, 0, line1_1, false); 
        ssd1306_print_str(60, 0, line1_2, false);
        ssd1306_print_str(4, 17, line2_1, false);
        ssd1306_print_str(60, 17, line2_2, false);

        ssd1306_print_str(4, 27, line3, false);
        ssd1306_print_str(4, 37, line4, false);
        ssd1306_print_str(4, 47, line5, false);

        ssd1306_display();
    }
}

void display_state(CarState state){
    switch (state){
        case ERROR:
            strcpy(line3, "Error");
            strcpy(line4, "Undefined actions");
            strcpy(line5, "Restart after 5s");
            break;
        case START_UP:
            strcpy(line3, "Safety");
            strcpy(line4, "");
            strcpy(line5, "");
            break;
        case ENGAGE_P_BRAKE_S:
            strcpy(line3, ">>Engage Park");
            strcpy(line4, "Brake");
            strcpy(line5, "");
            break;
        case RELEASE_P_BRAKE:
            strcpy(line3, ">>Release Park");
            strcpy(line4, "Brake before");
            strcpy(line5, "driving");
            break;
        case PULL_OVER_SAFELY:
            strcpy(line3, "Pull over");
            strcpy(line4, "safely");
            strcpy(line5, "");
            break;
        case ROLL_AWAY:
            strcpy(line3, "!!Roll away");
            strcpy(line4, ">>Shift to P");
            strcpy(line5, "or Hold brake");
            break;
        case ENGAGE_P_BRAKE_P:
            strcpy(line3, ">>Engage Parking");
            strcpy(line4, "for properly");
            strcpy(line5, "parking");
            break;
        case SHIFT_TO_P:
            strcpy(line3, ">>Shift to P");
            strcpy(line4, "for properly");
            strcpy(line5, "parking");
            break;
        case PARK_CORRECTLY:
            strcpy(line3, "Parked");
            strcpy(line4, "correctly");
            strcpy(line5, "");
            break;
        case DRIVING:
            strcpy(line3, "Driving");
            strcpy(line4, "");
            strcpy(line5, "");
            break;
        default:
            break;
    }
}
void display_infor(){
    //update gear to oled
    Gear gear = getGear();
    switch (gear){
        case P:
            strcpy(line1_1, "Gear:P");
            break;
        case N:
            strcpy(line1_1, "Gear:N");
            break;
        case R:
            strcpy(line1_1, "Gear:R");
            break;
        case D:
            strcpy(line1_1, "Gear:D");
            break;
        case S:
            strcpy(line1_1, "Gear:S");
            break;
        default:
            break;           
    }
    //update engine status
    if (getEngineOn() ){
        strcpy(line1_2, "Eng:1");
    } else{
        strcpy(line1_2, "Eng:0");
    }
    //update pBrake status
    if (getpBrakeOn() ){
        strcpy(line2_1, "PB:1");
    } else {
        strcpy(line2_1, "PB:0");
    }
    //update Brake status
    if (getBrakeOn() ) {
        strcpy(line2_2, "Br:1");
    } else {
        strcpy (line2_2, "Br:0");
    }
}
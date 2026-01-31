## MONITORING STARTUP AND PARKING PROCEDURES FOR NEW DRIVERS (CodeRace Challenge 2025 - BGSV)

## Introduction
This project implements a support system for monitoring startup and parking procedures for new drivers, based on decoding and analyzing CAN bus signals from a vehicle. The system continuously reads CAN messages, decodes key parameters (gear position, engine status, brake states, etc.), and determines the current operational state of the vehicle. It then displays real-time warnings and status information to the driver, helping ensure safe and correct operation - especially for beginners.

Compared to the Code Race 2025 version (which used Arduino for both simulation and main processing), this upgraded system uses an ESP32 microcontroller as the main processor. The ESP32 code is developed on the ESP-IDF framework and organized using an RTOS (FreeRTOS) architecture. State detection and transitions are managed by a dedicated state machine task, improving reliability and responsiveness. The Arduino is still used as a CAN signal simulator for testing and demonstration.

## Repository Structure

```
CodeRace2025weCan/
│
├── docs/                     
│   ├── images/
|   ├── reports/                
│   └── README.md
│
├── signal_simulate/             # Source code for CAN signals simulations (Arduino)
|   ├── README.md
│   └── signal_simulate.ino
│
├── src/                         # Main source code for ESP32 using IDF framework, organized with RTOS (FreeRTOS) architecture
│   ├── CMakeLists.txt
│   ├── README.md
│   ├── sdkconfig
│   ├── sdkconfig.old
│   ├── main/
│   │   ├── can_handler.c        # RTOS task: handles incoming CAN signals and updates system state
│   │   ├── can_handler.h
│   │   ├── fsm.c                # RTOS task: runs the warning state machine based on CAN signals for display
│   │   ├── fsm.h
│   │   ├── OLED_display.c       # RTOS task: displays system and warning info on the OLED screen
│   │   ├── OLED_display.h
│   │   ├── ssd1306_const.h
│   │   ├── ssd1306.c
│   │   ├── ssd1306.h
│   │   ├── StartParkAssist.c    # Entry point: initializes and adds RTOS tasks (CAN, OLED, FSM) to the system
│   │   ├── StartParkAssist.h
│   │   └── CMakeLists.txt
│   └── .vscode/
│
├── .vscode/ 
├── .gitignore
├── .gitattributes
└── README.md
```





# Main Source Code Overview (src)

This folder contains the core source code for the project, implemented on the ESP32 microcontroller using the ESP-IDF framework and FreeRTOS architecture. The code is designed for robust, real-time operation in automotive environments, focusing on safety and reliability.

## System Architecture

- **Platform:** ESP32 (ESP-IDF, FreeRTOS)
- **CAN Handling:** Utilizes the official TWAI library for CAN bus communication, ensuring compatibility and stability.
- **Task Structure:** The system is organized into multiple FreeRTOS tasks:
	- **CAN Handler Task:** Continuously receives and decodes CAN messages, updating global state variables (gear, engine, brakes, etc.).
	- **State Machine Task:** Implements a strict state machine to determine the current operational and warning state of the vehicle, based on decoded CAN data.
	- **OLED Display Task:** Updates the OLED screen in real time to show current status and warnings to the driver.

## Main Workflow

1. **Initialization:**
	 - The system initializes the CAN driver (TWAI), configures the hardware watchdog timer, and creates all main tasks.
2. **CAN Processing:**
	 - The CAN handler task listens for messages, decodes them according to the protocol used in the CodeRace2025 demo vehicle, and updates system state.
3. **State Management:**
	 - The state machine task evaluates the current state (gear, engine, brakes, etc.) and transitions between well-defined states (e.g., START_UP, DRIVING, PARK_CORRECTLY, ERROR, etc.).
	 - All state logic is centralized for clarity and safety. An explicit ERROR state is included to catch undefined or unexpected conditions, allowing the system to reset or recover safely.
4. **Display:**
	 - The OLED display task presents the current state and any warnings to the driver in real time.

## Safety and Reliability

- **Watchdog Timer:**
	- A hardware watchdog timer is configured and fed by all main tasks to prevent system hangs. If any task fails or the system becomes unresponsive, the watchdog will trigger a safe reset.
- **Error Handling:**
	- The state machine includes a dedicated ERROR state to handle undefined or abnormal conditions, ensuring the system can always return to a safe state.

## Code References

- Some functions and libraries in this project are adapted and improved from open-source resources. For original sources and credits, see:
	- [voidlooprobotech / ESP32_ESP-IDF_Code/15_SSD1306_ESP32](<https://github.com/voidlooprobotech/ESP32_ESP-IDF_Code/tree/main/15_SSD1306_ESP32>)

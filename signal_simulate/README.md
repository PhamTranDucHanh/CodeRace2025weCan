# Arduino CAN Signal Simulator

This folder contains Arduino code used to simulate CAN signals for testing and development. By pressing keypad buttons, the Arduino generates corresponding CAN messages that mimic real vehicle signals (gear position, engine status, brake states, etc.).

The signal formats and message structures here are designed to be compatible with the CAN protocol provided by the CodeRace2025 organizers, specifically matching the demo vehicle (Honda City 2018).

For a detailed explanation of how these signals are generated, proposed, and their exact structure (IDs, formats, etc.), please refer to Report 2 in the /data folder above. This report is accessible to all project members and collaborators.

The accuracy of this custom signal generation-based on official documentation-was validated by successful live demonstrations on the real vehicle during the competition.

Arduino is used for simulation to ensure quick setup and simplicity. This allows flexible, real-time testing of the main ESP32-based system without needing a real car or complex hardware.
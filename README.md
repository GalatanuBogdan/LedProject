# SmartLedController Arduino Project

SmartLedController is a project focused on automating the illumination of a staircase using LED strips and motion detection sensors. The code allows for setting the number of steps, the number of LEDs, a color, and the staircase will be automatically illuminated when motion is detected by one of the sensors placed in the rooms.

## Project Overview

The SmartLedController project aims to create an automated LED lighting system for a staircase using Arduino. The system includes motion detection sensors placed in different rooms to trigger the illumination of the LED strips on the staircase. The Arduino code controls the behavior of the LEDs based on the detected motion.

## Arduino Code

The Arduino code provided in this repository (`SmartLedController.ino`) is responsible for the functionality of the SmartLedController project. It includes the following main components:

### Libraries Used

- `FastLED`: Library for controlling LED strips.
- `time.h`: Library for time-related functions.
- `stdlib.h`: Standard C library for general-purpose functions.

### Main Variables

- `stairOnTime`: Time interval in seconds for how long the staircase should remain illuminated after motion detection.
- `detectionDistance`: Distance threshold in centimeters for motion detection.
- `numberOfStaircases`: Number of steps in the staircase.
- `delayBetweenStaircase`: Delay between illuminating each step.
- `ledPerStairCase`: Number of LEDs on each step.
- `numOfLeds`: Total number of LEDs in the LED strip.
- `ledOnStatus`: Current LED illumination status (0: off, 1: up->down, 2: down->up).
- Other auxiliary variables for managing timing and LED behavior.

### Functions

The code includes several functions to handle different aspects of the LED control and motion detection, including:

- Initializing LED strip, sensors, and color.
- Getting distances from motion detection sensors.
- Turning on/off LEDs for specific steps.
- Detecting motion on sensors.
- Starting and ending LED movement animations.

## Getting Started

1. Install the required libraries (`FastLED`) in your Arduino IDE.
2. Upload the `SmartLedController.ino` sketch to your Arduino board.
3. Connect the motion detection sensors and LED strip to the appropriate pins on the Arduino board as indicated in the code comments.
4. Adjust the variables in the code according to your setup (e.g., `stairOnTime`, `detectionDistance`, etc.).
5. Power on the Arduino and the LED strip. The LEDs will automatically illuminate when motion is detected by the sensors.

## Configuration

You can adjust the configuration variables at the beginning of the code to customize the behavior of the LED lighting system. Modify these variables to match your specific setup and preferences.

## Important Notes

- Make sure to handle the wiring and connections according to the comments in the code.
- This code is provided as a starting point and can be extended or modified for additional features or customizations.

## License

This project is licensed under the [MIT License](LICENSE).

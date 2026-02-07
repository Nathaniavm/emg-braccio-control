# emg-braccio-control
 
EMG Braccio Control is an embedded robotics project that enables controlling a TinkerKit Braccio robotic arm using EMG (Electromyography) muscle signals.

The system consists of two Arduino-based subsystems:

EMG Transmitter (TX) – Reads muscle signals and converts them into control commands.

Braccio Receiver (RX) – Receives commands and controls the robotic arm accordingly.


## The Responsibilities of The Components

| Component           | Description                              |
| ------------------- | ---------------------------------------- |
| EMG Sensors         | Capture muscle electrical activity       |
| Arduino TX          | Processes EMG signals and sends commands |
| Communication Layer | Transfers commands between Arduinos      |
| Arduino RX          | Receives commands and controls robot     |
| Braccio Arm         | Executes physical movement               |

## Required Hardware Components
Arduino Uno (TX)

Arduino Uno (RX)

TinkerKit Braccio robotic arm

EMG Click sensor module

Communication module

Power supply

## Software Requirements

Arduino IDE 2.x

Braccio Arduino library

Servo Arduino library

## Installation 

## System Architecture
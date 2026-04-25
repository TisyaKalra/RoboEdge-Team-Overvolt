# RoboEdge-Team-Overvolt
Intelligent control and sensor-based stabilization system for a 4WD multi-event robot (RoboRace, RoboObstacle, RoboSoccer) developed for RoboEdge – Team OverVolt

## Overview

This repository contains the control system developed by **Team OverVolt** for the RoboEdge competition at CELESTAI’26. The project focuses on building a robust and adaptive control architecture for a 4WD robot capable of performing across multiple events including RoboRace, RoboObstacle, and RoboSoccer.

The system integrates embedded control, sensor-based feedback, and optimized motor actuation to achieve stability, precision, and responsiveness in dynamic environments.

## Objective

To design and implement an intelligent control system that:

* Enables smooth and responsive robot motion
* Maintains stability during high-speed and uneven terrain operation
* Supports multiple operational modes for different event scenarios
* Provides a foundation for future Edge AI integration

## System Architecture

**(RC Receiver) → Signal Processing → Control Logic → Motor Actuation**

## Key Components:

* **Microcontroller:** ESP32-S3
* **Motor Drivers:** Dual BTS7960
* **Actuation:** 4× Johnson DC motors
* **Sensor:** MPU6050 (IMU for tilt detection)

## Core Features

### 1. Interrupt-Based RC Signal Processing

* Non-blocking PWM signal reading using hardware interrupts
* Ensures real-time responsiveness and efficient CPU utilization

### 2. Adaptive Input Mapping

* Deadzone implementation around neutral input
* Exponential (expo) scaling for fine low-speed control and aggressive high-speed response

### 3. Sensor-Based Stabilization

* MPU6050 IMU used for tilt detection
* Low-pass filtering applied to reduce noise
* Enables stabilization during uneven terrain navigation


### 4. Software-Based Motor Ramping

* Gradual change in motor speed to prevent sudden current spikes
* Protects motor drivers and battery
* Ensures smoother motion


### 5. Dual Mode Operation

* **Manual Mode:** Direct RC-based differential drive
* **Stabilized Mode:** Sensor-assisted control using IMU feedback


## Event Relevance

| Event            | System Contribution                                         |
| ---------------- | ----------------------------------------------------------- |
| **RoboRace**     | High-speed control with stability and smooth acceleration   |
| **RoboObstacle** | Balance, traction, and adaptive control over uneven terrain |
| **RoboSoccer**   | Precise low-speed control and directional responsiveness    |



## 🔌 Hardware Configuration

* ESP32-S3 Microcontroller
* 2 × BTS7960 Motor Drivers
* 4 × Johnson DC motors
* MPU6050 IMU Sensor
* FlySky RC Receiver
* 3S LiPo Battery (12V System)

## How It Works

1. RC receiver sends control signals to ESP32
2. Signals are processed using interrupt-based input capture
3. Control logic applies expo, deadzone, and stabilization
4. Output is sent to motor drivers
5. Motors execute differential drive motion


## Future Scope (Edge AI Integration)

The system is designed to support future enhancements such as:

* Vision-based object detection
* Autonomous navigation
* Edge AI inference using lightweight models
* Sensor fusion for improved decision-making

**Team OverVolt**
RoboEdge – CELESTAI’26

**Conclusion**
This project demonstrates a robust embedded control system designed for multi-event robotic applications. The architecture emphasizes stability, responsiveness, and scalability, providing a strong foundation for future AI-driven autonomous robotics.

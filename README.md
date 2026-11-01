# Autonomous Line Following Robot (LFR)

## 📌 Overview
A Line Following Robot (LFR) is an autonomous mobile robot designed to follow a predefined path—typically a high-contrast line (black on white or white on black) on the floor. It serves as a foundational project in robotics, embedded systems, and automation, bridging basic hardware integration with advanced control theory.

---

## 🛠️ System Architecture

| Subsystem | Component | Specifications & Role |
| :--- | :--- | :--- |
| **Control Unit** | Microcontroller Board | Processes 5-channel sensor data, executes the PID control algorithm, and outputs high-frequency PWM to the motor driver. |
| **Sensing** | 5-Channel IR Sensor Array | Provides differential positional feedback across the track to calculate the lateral error from the line. |
| **Motor Drive** | TB6612FNG Driver | MOSFET-based dual H-bridge motor driver delivering higher efficiency, lower heat dissipation, and precise PWM control compared to traditional L298N drivers. |
| **Actuation** | N20 Micro Gear Motors | High-RPM, compact DC motors providing the required power-to-weight ratio for rapid acceleration and tight cornering. |
| **Power & Chassis** | Custom Lightweight Frame & LiPo/Li-Ion Supply | Optimized for low center of gravity to prevent tipping during sharp differential turns. |

---

## ⚙️ Firmware Logic and Control Strategy

The core software pipeline runs an active PID loop to dynamically adjust differential wheel speeds:

```text
[ 5-Channel IR Sensor Array ]
              │
              ▼
[ Weighted Error Calculation ]
              │
              ▼
[ PID Control Logic (P + I + D) ]
              │
              ▼
[ Differential PWM Output ]
  ┌───────────┴───────────┐
  ▼                       ▼
(Left Motor Speed)     (Right Motor Speed)















  

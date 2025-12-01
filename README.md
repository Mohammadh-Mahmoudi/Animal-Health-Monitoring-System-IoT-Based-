# 🐾 Animal Health Monitoring System (IoT-Based)

An IoT-based health monitoring system designed to track vital signs, physical activity, and the location of animals in real-time.  
The system collects data from multiple sensors, processes it on a NodeMCU (ESP8266) microcontroller, and sends the results to **Blynk Cloud**, where the user can view all information through an intuitive GUI.

This project is designed for livestock and farm animals, but can be extended in the future for domestic animals (pets) as well.

---

## ✨ Features

- Real-time vital sign monitoring (Heart rate & SpO₂)
- Body temperature measurement
- Motion & activity tracking (resting, walking, running)
- Fall detection alert
- GPS location tracking
- Automatic wireless data transmission to Blynk Cloud
- Notifications and alerts for abnormal behaviour or emergency events


---

## 🧩 Hardware Components

| Component | Description |
|----------|-------------|
| **NodeMCU (ESP8266)** | Main microcontroller, handles all processing and WiFi communication |
| **MAX30102** | Measures heart rate and blood oxygen (SpO₂) |
| **MPU6050** | Accelerometer + gyroscope for activity recognition & fall detection |
| **DS18B20** | Digital temperature sensor (body temperature) |
| **GPS NEO-6M** | Provides animal’s real-time coordinates |
| **WiFi Router** | Connects NodeMCU to the internet |
| **Blynk Cloud** | Receives processed data and visualises it in a mobile GUI |

---

## 🏗️ System Architecture Overview


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
| **Blynk Cloud** | Receives processed data and visualises it in a GUI |

---

## 🏗️ System Architecture Overview


<img width="539" height="774" alt="chrome_BCA9CNWMy3" src="https://github.com/user-attachments/assets/813f5338-693f-40e9-a9d9-adf93952731b" />

---

## 📡 How It Works

### **1. Sensor Data Acquisition**
- **MAX30102** captures IR & RED light to estimate pulse rate and oxygen saturation.  
- **MPU6050** measures acceleration and angular velocity to classify movement:  
  - RESTING  
  - WALKING  
  - RUNNING  
  - FALL DETECTED (ALERT)  
- **DS18B20** provides accurate body temperature.  
- **GPS NEO-6M** tracks the real-time position.

---

### **2. Local Processing on NodeMCU**
The ESP8266 performs:

- Filtering of raw MAX30102 signals  
- SpO₂ calculation using the R-ratio method  
- Adaptive averaging for stable BPM  
- Motion classification using acceleration differences  
- Fall detection using threshold peaks  
- GPS timestamp comparison to detect the freshness of location data  
- Periodic sensor timers  
- Error handling & safe defaults  

---

### **3. IoT Communication Layer**
The system connects through:

- **ESP8266 WiFi** → local router  
- **Blynk Cloud** → remote visualization  

Data is sent using:

- `Blynk.virtualWrite()` for every sensor channel  
- `Blynk.logEvent()` for fall alerts  
- Built-in Blynk datastreams for GPS, BPM, SpO₂, temperature, and activity state  

---

### **4. User Interface (Blynk Dashboard)**

Blynk GUI displays:

- 📍 GPS Location (coordinate)  
- ❤️ Heart Rate  
- 🫁 SpO₂ level  
- 🌡️ Temperature (°C)  
- 🏃 Activity State (Running / Walking / Resting)  
- ⚠️ Fall Detection LED  
- 🐾 Last Update Times

  

<img width="1913" height="926" alt="KMPlayer64_MMjwJhF2SS" src="https://github.com/user-attachments/assets/1aa6875e-8e12-4c51-ae6e-ad780bdfb50a" />

---


## 🎥 Demo Video
👉 Insert your demo video link here:  

`https://github.com/USERNAME/REPO/assets/...`

(To upload: go to Issues → New Issue → drag the video → copy generated link.)

---

## 🖼 Device Photos


<img width="606" height="1068" alt="KMPlayer64_85ZNYPsI3y" src="https://github.com/user-attachments/assets/f935964f-d54b-4b91-985b-e9e7d33f775b" />






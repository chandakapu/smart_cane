# Smart Cane with Fall Detection using ESP32 & MQTT

## Overview
This project is a smart cane system built using an **ESP32** microcontroller and an **MPU6050** sensor. The cane detects falls using acceleration and gyroscope data and sends real-time alerts to an MQTT broker. The broker relays notifications to caregivers, ensuring timely assistance. The system uses **SSL/TLS-secured MQTT** for safe communication.

## Features
- **Real-time fall detection** using MPU6050 sensor data.
- **Secure MQTT communication** using HiveMQ Cloud.
- **Wi-Fi connectivity** for seamless data transmission.
- **Customizable detection thresholds** for different sensitivity levels.
- **Test message on MQTT connection** to verify broker connectivity.

## Components Required
- **ESP32** development board
- **MPU6050** accelerometer and gyroscope sensor
- **Wi-Fi connection**
- **MQTT broker (HiveMQ Cloud)**
- **Buzzer/LED (Optional for local alerts)**

## Installation & Setup

### 1. Install Required Libraries
Ensure you have the following libraries installed in the **Arduino IDE**:
- `WiFi.h` (Built-in for ESP32)
- `WiFiClientSecure.h`
- `PubSubClient.h`
- `Wire.h`
- `Adafruit_MPU6050.h`
- `Adafruit_Sensor.h`

You can install them via the **Library Manager** in Arduino IDE.

### 2. Clone the Repository
```sh
git clone https://github.com/chandakapu/smart_cane.git
cd smart-cane-mqtt
```

### 3. Configure Wi-Fi and MQTT Credentials
Edit the following placeholders in `smart_cane.ino`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER";
const char* mqtt_username = "YOUR_MQTT_USERNAME";
const char* mqtt_password = "YOUR_MQTT_PASSWORD";
const char* mqtt_topic = "YOUR_MQTT_TOPIC";
```

### 4. Upload the Code to ESP32
1. Open **Arduino IDE**.
2. Select the **ESP32 board** under `Tools` > `Board`.
3. Connect your ESP32 and select the correct port.
4. Click **Upload**.
5. Open the **Serial Monitor** to check the connection status.

### 5. Verify MQTT Connection
Once connected, you should see messages in the Serial Monitor indicating Wi-Fi and MQTT connection success. A test message will be published to verify broker connectivity.

## How It Works
1. The **MPU6050** sensor continuously monitors acceleration and gyroscope data.
2. If a **free fall** is detected (low acceleration), the system enters fall detection mode.
3. If a **strong impact** is detected (high acceleration after free fall), the system checks for stillness.
4. If **no movement** is detected for a predefined duration, an **MQTT alert is sent**.
5. The caregiver receives the alert via an MQTT subscriber (e.g., a mobile app or web dashboard).

## MQTT Broker Configuration
This project is tested with **HiveMQ Cloud**. You can create a free account and get your **host, port (8883 for SSL), username, and password**.

## Example MQTT Message
```json
{
  "alert": "Fall detected!",
  "time": 1700000000
}
```

## Future Improvements
- Implement **mobile notifications** via an MQTT subscriber app.
- Add **local alerts** (buzzer/vibration feedback).
- Extend functionality to detect **irregular walking patterns**.

## License
This project is open-source under the **MIT License**.

---

📌 GitHub Repository: [smart_cane](https://github.com/Candaka/smart_cane)


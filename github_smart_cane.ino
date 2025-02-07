#include <WiFi.h>  // Required for ESP32 WiFi
#include <WiFiClientSecure.h>  // For secure (SSL/TLS) connections
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// WiFi Credentials (Replace with your actual credentials before running)
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// HiveMQ Cloud Broker Details (Replace with your actual credentials before running)
const char* mqtt_server = "your_mqtt_server";
const int mqtt_port = 8883;  // Secure MQTT port
const char* mqtt_username = "your_mqtt_username";
const char* mqtt_password = "your_mqtt_password";
const char* mqtt_topic = "smartcane/fall";  // MQTT Topic for fall alerts

// Create a WiFiClientSecure object for SSL/TLS connection
WiFiClientSecure espClient;
// Create a PubSubClient object using the secure client
PubSubClient client(espClient);
// Create an MPU6050 object
Adafruit_MPU6050 mpu;

// Calibration offsets
float accX_offset = 0.70, accY_offset = -0.01, accZ_offset = -0.29;
float gyroX_offset = 0.71, gyroY_offset = 0.01, gyroZ_offset = -0.03;

// Fall detection thresholds
#define FREE_FALL_THRESHOLD 4.0    // Acceleration threshold (m/s²)
#define IMPACT_THRESHOLD 15.0      // Impact detection threshold (m/s²)
#define STILLNESS_THRESHOLD 0.5    // Gyro threshold (rad/s)
#define STILLNESS_DURATION 3000    // Time to check stillness (ms)

bool inFreeFall = false;
unsigned long fallStartTime = 0;
unsigned long impactTime = 0;

void setupWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Connecting to HiveMQ...");
        if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT Broker!");
            
            // Send a test message to verify connectivity
            if (client.publish(mqtt_topic, "Test connection established!")) {
                Serial.println("✅ Test message sent to MQTT broker.");
            } else {
                Serial.println("❌ Failed to send test message.");
            }
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void sendFallAlert() {
    String alertMessage = "{ \"alert\": \"Fall detected!\", \"time\": " + String(millis()) + " }";
    client.publish(mqtt_topic, alertMessage.c_str());
    Serial.println("🚨 MQTT Alert Sent: " + alertMessage);
}

void setup() {
    Serial.begin(115200);
    setupWiFi();
    
    // Allow SSL/TLS connection without certificate verification (for testing)
    espClient.setInsecure();

    client.setServer(mqtt_server, mqtt_port);

    if (!mpu.begin()) {
        Serial.println("MPU6050 not found! Check wiring.");
        while (1);
    }
    Serial.println("MPU6050 Connected!");

    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Apply calibration offsets
    float correctedAcX = a.acceleration.x - accX_offset;
    float correctedAcY = a.acceleration.y - accY_offset;
    float correctedAcZ = a.acceleration.z - accZ_offset;
    
    float correctedGyX = g.gyro.x - gyroX_offset;
    float correctedGyY = g.gyro.y - gyroY_offset;
    float correctedGyZ = g.gyro.z - gyroZ_offset;

    // Calculate total acceleration magnitude
    float totalAcceleration = sqrt(correctedAcX * correctedAcX +
                                   correctedAcY * correctedAcY +
                                   correctedAcZ * correctedAcZ);

    // Calculate total gyroscope movement
    float totalGyro = sqrt(correctedGyX * correctedGyX +
                           correctedGyY * correctedGyY +
                           correctedGyZ * correctedGyZ);

    // Step 1: Detect Free Fall
    if (totalAcceleration < FREE_FALL_THRESHOLD && !inFreeFall) {
        inFreeFall = true;
        fallStartTime = millis();
        Serial.println("🚨 Free Fall Detected!");
    }

    // Step 2: Detect Impact
    if (inFreeFall && totalAcceleration > IMPACT_THRESHOLD) {
        impactTime = millis();
        Serial.println("💥 Impact Detected! Possible Fall.");
        inFreeFall = false;
    }

    // Step 3: Check Stillness
    if (impactTime > 0 && (millis() - impactTime > STILLNESS_DURATION)) {
        if (totalGyro < STILLNESS_THRESHOLD) {
            Serial.println("⚠️ User is immobile after fall! Sending Alert...");
            sendFallAlert();  // Send MQTT alert
        } else {
            Serial.println("✅ User is moving, no alert needed.");
        }
        impactTime = 0;
    }

    delay(100);
}

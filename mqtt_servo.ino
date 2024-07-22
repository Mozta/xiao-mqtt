#include <WiFi.h>
#include <MQTT.h>
#include <ESP32Servo.h>

// Define WiFi credentials
const char ssid[] = "CHANGE_ME";
const char pass[] = "CHANGE_ME";

// Define MQTT server
const char mqttServer[] = "mqtt.eclipseprojects.io";

// Define MQTT topic
const char mqttTopic[] = "FAB24/test";

// Create WiFi and MQTT clients
WiFiClient net;
MQTTClient client;

// Define the servo pins
const int servoPin = D0;  // GPIO pin for the X-axis servo
const int servoYPin = D1; // GPIO pin for the Y-axis servo

// Create servo objects
Servo myServo;
Servo myServoY;

unsigned long lastMillis = 0;

void connectWiFi()
{
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nWiFi connected!");
}

void connectMQTT()
{
    Serial.print("Connecting to MQTT...");
    while (!client.connect("arduino", "public", "public"))
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nMQTT connected!");
    client.subscribe(mqttTopic);
}

void messageReceived(String &topic, String &payload)
{
    Serial.println("Incoming: " + topic + " - " + payload);

    // Split the payload and convert to integers
    int commaIndex = payload.indexOf(',');
    if (commaIndex != -1)
    {
        String xStr = payload.substring(0, commaIndex);
        String yStr = payload.substring(commaIndex + 1);

        int x = xStr.toInt();
        int y = yStr.toInt();

        if (x >= 0 && x <= 180 && y >= 0 && y <= 180)
        {
            myServo.write(x);
            myServoY.write(y);
            Serial.print("Moving X,Y servo to: ");
            Serial.println(payload);
            // Serial.print("Moving Y servo to: ");
            // Serial.println(y);
        }
    }
    else
    {
        Serial.println("Invalid payload format. Expected 'x,y'.");
    }
}

void setup()
{
    // Initialize serial communication
    Serial.begin(115200);

    // Attach the servos to the pins
    myServo.attach(servoPin);
    myServoY.attach(servoYPin);

    // Initialize the servos to the center position
    myServo.write(90);
    myServoY.write(90);
    Serial.println("Servos initialized to center position.");

    // Start WiFi and MQTT connections
    connectWiFi();
    client.begin(mqttServer, net);
    client.onMessage(messageReceived);
    connectMQTT();
}

void loop()
{
    client.loop();
    delay(10); // Fixes some issues with WiFi stability

    if (!client.connected())
    {
        connectMQTT();
    }
}

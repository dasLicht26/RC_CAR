#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

Servo lenkservo;
Servo motorESC;

const int servoPin = 27;
const int escPin = 25;
unsigned long lastCommandTime = 0;
const unsigned long timeout = 1000;

// JSON Puffer
char incomingData[250];

// Default-Werte
int servoWinkel = 90;
int escPWM = 1500;
bool ledStatus = false;

// Callback bei Empfang
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, incomingData, len);

    if (!error) {
        // Lenkung
        if (doc.containsKey("servo")) {
            int winkel = doc["servo"];
            if (winkel >= 0 && winkel <= 180) {
                servoWinkel = winkel;
                lenkservo.write(winkel);
                Serial.print("Servo: "); Serial.println(winkel);
            }
        } else {
            lenkservo.write(90);
        }

        // ESC
        if (doc.containsKey("esc")) {
            int pwm = doc["esc"];
            if (pwm >= 1000 && pwm <= 2000) {
                escPWM = pwm;
                motorESC.writeMicroseconds(pwm);
                Serial.print("ESC: "); Serial.println(pwm);
            }
        } else {
            motorESC.writeMicroseconds(1500);
        }

        // LED
        if (doc.containsKey("led")) {
            ledStatus = doc["led"];
            digitalWrite(LED_BUILTIN, ledStatus ? HIGH : LOW);
            Serial.print("LED: "); Serial.println(ledStatus ? "ON" : "OFF");
        } else {
            digitalWrite(LED_BUILTIN, LOW);
        }

        lastCommandTime = millis();
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    WiFi.mode(WIFI_STA);
    Serial.print("ESP-NOW MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_recv_cb(onDataRecv);

    ESP32PWM::allocateTimer(0);
    lenkservo.setPeriodHertz(50);
    lenkservo.attach(servoPin, 500, 2500);
    lenkservo.write(servoWinkel);

    motorESC.setPeriodHertz(50);
    motorESC.attach(escPin, 1000, 2000);
    motorESC.writeMicroseconds(escPWM);
}

void loop() {
    if (millis() - lastCommandTime > timeout) {
        motorESC.writeMicroseconds(1500);
    }
}

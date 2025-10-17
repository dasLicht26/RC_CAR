#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>

uint8_t receiver[] = {0x78, 0x42, 0xC1, 0x6B, 0x39, 0x0C};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peer{};
  memcpy(peer.peer_addr, receiver, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);

  Serial.println("Bridge bereit – empfange JSON über USB");
}

void loop() {
  if (Serial.available()) {
    String json = Serial.readStringUntil('\n');
    if (json.length() > 5) {
      esp_now_send(receiver, (uint8_t*)json.c_str(), json.length());
      Serial.println("Gesendet: " + json);
    }
  }
}
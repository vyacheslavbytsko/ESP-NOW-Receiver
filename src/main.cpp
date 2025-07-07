#include <WiFi.h>
#include <esp_now.h>

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    Serial.print("Received from: ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", mac[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.print(" | Data: ");
    Serial.println((char *)incomingData);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_recv_cb(onReceive);
}

void loop() {
    delay(100); // Ничего не делаем, просто ждём
}
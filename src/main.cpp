#include <WiFi.h>
#include <esp_now.h>

// Переменные для отслеживания потерь
unsigned long lastReceivedNumber = 0;
unsigned long total = 0;
unsigned long expe = 0;
bool firstPacket = true;

struct Data {
  uint16_t heartbeat;
  float temperature;
  float oxygen;
  uint32_t timestamp;
};

Data currentData;
Data prevData;

void onReceive(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    if (len != 29) return;

    uint8_t i = incomingData[0];

    // prevData
    memcpy(&prevData.timestamp,   &incomingData[1],  4);
    memcpy(&prevData.heartbeat,   &incomingData[5],  2);
    memcpy(&prevData.oxygen,      &incomingData[7],  4);
    memcpy(&prevData.temperature, &incomingData[11], 4);

    // currentData
    memcpy(&currentData.timestamp,   &incomingData[15],  4);
    memcpy(&currentData.heartbeat,   &incomingData[19], 2);
    memcpy(&currentData.oxygen,      &incomingData[21], 4);
    memcpy(&currentData.temperature, &incomingData[25], 4);

    Serial.printf("Packet #%d\n", i);
    Serial.printf("Prev  | HR=%u | O2=%.2f | T=%.1f | ts=%lu\n",
                  prevData.heartbeat, prevData.oxygen,
                  prevData.temperature, prevData.timestamp);

    Serial.printf("Curr  | HR=%u | O2=%.2f | T=%.1f | ts=%lu\n",
                  currentData.heartbeat, currentData.oxygen,
                  currentData.temperature, currentData.timestamp);
}

void setup() {
    Serial.begin(115200);

    // Включаем ESP NOW
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    // Регистрируем коллбэк на получение
    esp_now_register_recv_cb(onReceive);
}

void loop() {}
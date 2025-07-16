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

Data outData;

void onReceive(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    if (len != 15) return;

    uint8_t i = incomingData[0];

    memcpy(&outData.timestamp,  &incomingData[1],  4); // uint32_t
    memcpy(&outData.heartbeat,  &incomingData[5],  2); // uint16_t
    memcpy(&outData.oxygen,     &incomingData[7],  4); // float
    memcpy(&outData.temperature,&incomingData[11], 4); // float

    Serial.printf("Packet #%d | HR=%u | O2=%.2f | T=%.1f | ts=%lu\n",
                  i, outData.heartbeat, outData.oxygen, outData.temperature, outData.timestamp);
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
#include <WiFi.h>
#include <esp_now.h>

// Переменные для отслеживания потерь
unsigned long lastReceivedNumber = 0;
unsigned long total = 0;
unsigned long expe = 0;
bool firstPacket = true;

void onReceive(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    // Преобразуем полученные данные в число
    unsigned long currentNumber = strtoul((char *)incomingData, NULL, 10);
    
    Serial.print("Received from: ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", mac_addr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.print(" | Data: ");
    Serial.print(currentNumber);
    
    // Логика подсчета потерь
    if (firstPacket) {
        // Первый пакет - инициализация
        lastReceivedNumber = currentNumber;
        total = 1;
        expe = 1;
        firstPacket = false;
        Serial.print(" | First packet");
    } else {
        // Проверяем, идут ли числа в обратную сторону
        if (currentNumber < lastReceivedNumber) {
            // Обнаружен обратный ход - сбрасываем счетчики
            Serial.print(" | Reverse detected - resetting counters");
            lastReceivedNumber = currentNumber;
            total = 1;
            expe = 1;
        } else {
            // Нормальная последовательность
            total++;
            
            // Вычисляем количество ожидаемых пакетов
            expe = expe + currentNumber - lastReceivedNumber;

            // Вычисляем процент потерь
            float lossPercentage = 0.0;
            if (expe > 0) {
                lossPercentage = ((float)(total) / (float)expe) * 100.0;
            }

            Serial.print(" | Expected: ");
            Serial.print(expe);
            Serial.print(" | Received: ");
            Serial.print(total);
            Serial.print(" | RSSI: ");
            Serial.print(lossPercentage, 2);
            Serial.print("%");
            lastReceivedNumber = currentNumber;

            if(total >= 100){
                firstPacket = true;
            }
        }
    }

    Serial.println();
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
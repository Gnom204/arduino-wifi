#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h> // Необходимо установить библиотеку

const char* ssid = "Kaif";
const char* password = "11111111";
const char* serverUrl = "http://37.252.17.105/api/data/";

WiFiClient wifiClient;

// const String JsonT= {
// "name":"Тест",
// "sound":123,
// "humidity":322,
// "temperature":27,
// "dust":0.005,
// "gas":23
// };

void setup() {
  Serial.begin(115200);
  
  // Подключение к WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  if (Serial.available()) {
    String payload = Serial.readStringUntil('\n');
    payload.trim();
    
    if (payload.length() > 0 && WiFi.status() == WL_CONNECTED) {
      Serial.print("Получены данные: ");
      Serial.println(payload);

      // Преобразование в JSON
      String jsonBody = convertToJson(payload);
      Serial.println(jsonBody);
      if(jsonBody.length() > 0) {
        HTTPClient http;
        http.begin(wifiClient, serverUrl);
        http.addHeader("Content-Type", "application/json");
        
        int httpCode = http.POST(jsonBody);
        
        http.end();
      }
    }
  }
  delay(2000);
}

String convertToJson(String payload) {
  DynamicJsonDocument doc(512); // Выделяем память для JSON
  
  int from = 0;
  while(from < payload.length()) {
    int eqPos = payload.indexOf('=', from);
    if(eqPos == -1) break;
    
    String key = payload.substring(from, eqPos);
    key.trim();
    
    int ampPos = payload.indexOf('&', eqPos + 1);
    if(ampPos == -1) ampPos = payload.length();
    
    String value = payload.substring(eqPos + 1, ampPos);
    value.trim();

    // Конвертация числовых значений для специфичных ключей
    if(key == "temperature"  
       key == "humidity"  
       key == "sound"  
       key == "dust"  
       key == "gas") {
      doc[key] = value.toFloat();
    } else {
      doc[key] = value;
    }
    
    from = ampPos + 1;
  }

  String result;
  serializeJson(doc, result);
  return result;
}
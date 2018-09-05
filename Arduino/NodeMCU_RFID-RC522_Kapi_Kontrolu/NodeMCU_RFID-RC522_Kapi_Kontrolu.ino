#include <ESP8266WiFi.h>
#include <SPI.h>
#include "MFRC522.h"
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

/* Pin Bağlantıları

  RFID RC522 => NodeMCU 8266

    SDA     =   GPIO4  / D2
    SCK     =   GPIO14 / D5
    MOSI    =   GPIO13 / D7
    MISO    =   GPIO12 / D6
    IRQ     =
    GND     =   GND
    RST     =   GPIO5 / D1
    3.3V    =   3.3V


  ROLE => NodeMCU 8266

    GND     =   GND
    3.3V    =   3.3V
    S       =   GPIO2 / D4
*/

// Wifi Bağlantısı
const char *ssid =  "MakdosHQ";
const char *pass =  "Makdos.2017.";

// API Url
// const char *apiUrl =  "http://api.kodz.org/mcu/rfiddoor/sorgu.php?card_uid=";
const char *apiUrl =  "http://185.122.200.14/api/control/?door_id=1&card_identity=";


#define RST_PIN  5 // GPIO5 / D1
#define SS_PIN  4 // GPIO4  / D2
MFRC522 mfrc522(SS_PIN, RST_PIN);
int ROLE = 2; // GPIO2 / D4

void setup() {

  pinMode(ROLE, OUTPUT);

  Serial.begin(115200);
  delay(250);
  Serial.println(F("Baglaniyor...."));

  SPI.begin();
  mfrc522.PCD_Init();

  WiFi.begin(ssid, pass);

  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (retries < 10)) {
    retries++;
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi Bağlandı."));
  }

  Serial.println(F("Hazır!"));
  Serial.println(F("======================================================"));
  Serial.println(F("Lütfen Kartınızı Gösteriniz"));
}

void loop() {

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(50);
    return;
  } else {
    unsigned long uid = getID();
    if (uid != -1) {
      Serial.println(F("======================================================"));
      Serial.print("Kart Kimliği: ");
      Serial.println(uid);

      HttpSend(String(uid));
    }
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }
}


void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

unsigned long getID() {
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return -1;
  }
  unsigned long hex_num;
  hex_num =  mfrc522.uid.uidByte[0] << 24;
  hex_num += mfrc522.uid.uidByte[1] << 16;
  hex_num += mfrc522.uid.uidByte[2] <<  8;
  hex_num += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA(); // Stop reading
  return hex_num;
}

void HttpSend(String card_uid) {
  HTTPClient http;

  http.begin(apiUrl + card_uid);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String result = http.getString();

    // JSON Parsing
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(result);

    // Parameters
    boolean status = root["status"];
    const char* card_identity = root["card_identity"];
    const char* door = root["door"];
    const char* message = root["message"];
    const char* reason = root["reason"];
    const char* personnel_name = root["personnel_name"];


    // Role Kontrolü
    if (status) {
      digitalWrite(ROLE, HIGH);
      delay(1000);
      digitalWrite(ROLE, LOW);
    }

    Serial.print("Personel: "); Serial.println(personnel_name);
    Serial.print("Kapı: "); Serial.println(door);
    Serial.print("Mesaj: "); Serial.println(message);
    Serial.print("Sebep: "); Serial.println(reason);
  } else {
    Serial.println("Internet Baglantisi Yok. Tekrar Deneyiniz.");
    delay(100);
    return;
  }
  Serial.println(F("======================================================"));
  Serial.println(F("======================================================"));

  http.end();
}

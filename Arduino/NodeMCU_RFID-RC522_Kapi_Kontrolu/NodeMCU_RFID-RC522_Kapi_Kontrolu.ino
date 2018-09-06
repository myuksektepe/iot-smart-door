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

// WIFI SSID (ADI)
const char *ssid =  "MakdosHQ";

// WIFI PAROLASI
const char *pass =  "Makdos.2017.";

// KAPI ID'si
String door_id = "1";

// API URL
String apiUrl = "http://185.122.200.14/api/control/?door_id=" + door_id + "&card_identity=";


#define RST_PIN  5
#define SS_PIN  4
MFRC522 mfrc522(SS_PIN, RST_PIN);
int ROLE = 2;

void setup() {

  pinMode(ROLE, OUTPUT);

  Serial.begin(115200);
  delay(250);
  Serial.print(F("WIFI Baglaniyor"));

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

    Serial.println(F(""));
    Serial.println(F("WiFi Baglantisi BASARILI!"));
    Serial.println(F("Her Sey Hazir!"));
    Serial.println(F("======================================================"));
    Serial.println(F("Lutfen Kartinizi Gosteriniz."));

  } else {

    Serial.println(F("WiFi Baglantisi BASARISIZ!"));
    Serial.println(F("Lutfen baglanti icin girdiginiz degerlerin dogrulundan emin olun."));

  }
}

void loop() {

  // HER YARIM SANIYEDE BIR YENI KART SORGULA
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(500);
    return;

    // EGER BIR KART OKUTULURSA
  } else {

    // KART ID'SINI TESPIT ET
    unsigned long card_identity = getID();

    if (card_identity != -1) {
      Serial.println(F("======================================================"));
      Serial.print("Kart Kimliği: ");
      Serial.println(card_identity);


      // INTERNET BAGLANTISI VARSA
      if (WiFi.status() == WL_CONNECTED) {

        // KART ID'SINI API'DA SORGULA
        HttpSend(String(card_identity));

        // INTERNET BAGLANTISI YOKSA
      } else {

        if (card_identity == 2697911972) {
          // ROLE YONETIMI
          Serial.println(F("======================================================"));
          Serial.println(F("Gecis Iznı Verildi. (Semih Basoglu)"));
          digitalWrite(ROLE, HIGH);
          delay(1000);
          digitalWrite(ROLE, LOW);

        }
      }
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
  mfrc522.PICC_HaltA(); // Okumayı durdur
  return hex_num;
}

void HttpSend(String card_identity) {
  HTTPClient http;

  http.begin(apiUrl + card_identity);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String result = http.getString();

    // JSON Parçalama
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& json_object = jsonBuffer.parseObject(result);

    // Parametreler
    boolean status = json_object["status"];
    const char* card_identity = json_object["card_identity"];
    const char* door = json_object["door"];
    const char* message = json_object["message"];
    const char* reason = json_object["reason"];
    const char* personnel_name = json_object["personnel_name"];


    // Role Yönetimi
    if (status) {
      digitalWrite(ROLE, HIGH);
      delay(1000);
      digitalWrite(ROLE, LOW);
    }

    // JSON'dan Donen Sonuclar
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

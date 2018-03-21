#include <SPI.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define DHTPIN D3 // what digital pin we're connected to
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;

#define OLED_RESET LED_BUILTIN //4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define ssid "iPhone"
#define password "12344145"

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
char auth[] = "6ce501a3f6a04e13b74d43272f330867";
int sense_Pin = 0; // sensor input at Analog pin A0
int moi_value = 0;
float h = 0;
float t = 0;
uint16_t lux;
void uploadThingsSpeak(float moi, float hum, float temp);

void setup()   {                
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  dht.begin();
  WiFi.begin(ssid, password);
  Blynk.begin(auth, ssid, password);
  Wire.begin(D2, D1);
  lightMeter.begin();
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  display.display();
  delay(2000);

  display.clearDisplay();


  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

}


void loop() {
  display.clearDisplay();
  display.setCursor(0,0);
  // หน่วงเวลา 2 วินาทีให้เซนเซอร์ทำงาน
  delay(2000);
  h = dht.readHumidity();
  t = dht.readTemperature();
  float f = dht.readTemperature(true);
  // เช็คถ้าอ่านค่าไม่สำเร็จให้เริ่มอ่านใหม่
  if (isnan(h) || isnan(t) || isnan(f)) {
  Serial.println("Failed to read from DHT sensor!");
    return;
  }
  lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  
  moi_value= analogRead(sense_Pin);
  moi_value= moi_value/10;
  Serial.print("MOISTURE LEVEL : ");
  Serial.println(moi_value);
  
  Serial.println(WiFi.localIP());
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t\n");
  
  
  display.print("Hum:");
  display.println(h); 
  
  display.print("TeC:");
  display.println(t); 

  display.print("Moi: ");
  display.println(moi_value);

  display.print("Lig: ");
  display.println(lux);
  
  display.display();
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V7, lux);
  Blynk.virtualWrite(V8, moi_value);
  Blynk.run();
  // uploadThingsSpeak(moi_value, h, t);
}
void uploadThingsSpeak(float moi, float hum, float temp) {
    static const char* host = "api.thingspeak.com"; 
    static const char* apiKey = "B17ZDPNJ580FUZTS"; 

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        DEBUG_PRINTLN("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/update/";
    url += "?key=";
    url += apiKey;
    url += "&field1=";
    url += moi;
    url += "&field2=";
    url += hum;
    url += "&field3=";
    url += temp;
    url += "&field4=";
    url += lux;

    DEBUG_PRINT("Requesting URL: ");
    DEBUG_PRINTLN(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
}



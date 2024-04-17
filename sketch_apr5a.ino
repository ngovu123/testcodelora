#define BLYNK_TEMPLATE_ID "TMPL6rFOZ6SM1"
#define BLYNK_TEMPLATE_NAME "esp8266loratest111"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BlynkEdgent.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* host = "script.google.com";
const int httpsPort = 443;

String GAS_ID = "AKfycbwr_3bSYXaLvfctHlU4P_4mJaxSqJR4fIVe0GiZ56Cn8vNqaQqObUkfhHshCxWKPo1S"; // ID của Google Apps Script

const char* ssid = "Nora";
const char* password = "0967230939";

WiFiClientSecure client;

#define ss 15
#define rst 16
#define dio0 4

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String counter;
String soilMoistureValue;
String soilmoisturepercent;
String soiltemp;
String temperature;
String humidity;

void setup() {
  Serial.begin(115200);
  BlynkEdgent.begin();
  while (!Serial);

  Serial.println("LoRa Receiver");

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  display.clearDisplay();

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  client.setInsecure();
}

void loop() {
  BlynkEdgent.run();
  // try to parse packet
  int pos1, pos2, pos3, pos4, pos5;
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // received a packet
    Serial.print("Received packet:  ");
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData);
    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    pos1 = LoRaData.indexOf('/');
    pos2 = LoRaData.indexOf('&');
    pos3 = LoRaData.indexOf('#');
    pos4 = LoRaData.indexOf('@');
    pos5 = LoRaData.indexOf('$');

    counter = LoRaData.substring(0, pos1);
    soilMoistureValue = LoRaData.substring(pos1 + 1, pos2);
    soilmoisturepercent = LoRaData.substring(pos2 + 1, pos3);
    soiltemp = LoRaData.substring(pos3 + 1, pos4);
    temperature = LoRaData.substring(pos4 + 1, pos5);
    humidity = LoRaData.substring(pos5 + 1, LoRaData.length());

    //send data to blynk
    Blynk.virtualWrite(V1, soilmoisturepercent); //Soil Moisture
    Blynk.virtualWrite(V2, soiltemp); //Soil Temperature
    Blynk.virtualWrite(V3, temperature);  // for Temperature
    Blynk.virtualWrite(V4, humidity);  //for Humidity

    Serial.print(F("Packet No = "));
    Serial.println(counter);

    Serial.print(F("Soil Moisture: "));
    Serial.print(soilmoisturepercent);
    Serial.println(F("%"));

    Serial.print(F("Soil Temperature: "));
    Serial.print(soiltemp);
    Serial.println(F("°C"));

    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));

    Serial.print(F("Humidity = "));
    Serial.print(humidity);
    Serial.println(F("%"));

    Serial.print("Soil Moisture Value: ");
    Serial.print(soilMoistureValue);

    Serial.println();

    if (soilmoisturepercent.toInt() > 100)
    {
      display.clearDisplay();

      // display Soil Humidity
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(0, 5);
      display.print("RH of Soil: ");
      display.print("100");
      display.print(" %");

      // display soil temperature
      display.setCursor(0, 20);
      display.print("Soil Temp: ");
      display.print(soiltemp);
      display.print(" ");
      display.cp437(true);
      display.write(167);
      display.print("C");

      // display air temperature
      display.setCursor(0, 35);
      display.print("Air Temp: ");
      display.print(temperature);
      display.print(" ");
      display.cp437(true);
      display.write(167);
      display.print("C");

      // display relative humidity of Air
      display.setCursor(0, 50);
      display.print("RH of Air: ");
      display.print(humidity);
      display.print(" %");

      display.display();
      delay(1500);
    }
    else if (soilmoisturepercent.toInt() < 0)
    {
      display.clearDisplay();

      // display Soil Humidity
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(0, 5);
      display.print("RH of Soil: ");
      display.print("0");
      display.print(" %");

      // display soil temperature
      display.setCursor(0, 20);
      display.print("Soil Temp: ");
      display.print(soiltemp);
      display.print(" ");
      display.cp437(true);
      display.write(167);
      display.print("C");

      // display air temperature
      display.setCursor(0, 35);
      display.print("Air Temp: ");
      display.print(temperature);
      display.print(" ");
      display.cp437(true);
      display.write(167);
      display.print("C");

      // display relative humidity of Air
      display.setCursor(0, 50);
      display.print("RH of Air: ");
      display.print(humidity);
      display.print(" %");

      display.display();
      delay(1500);
    }
    else if (soilmoisturepercent.toInt() >= 0 && soilmoisturepercent.toInt() <= 100)
    {
      display.clearDisplay();

      // display Soil humidity
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(0, 5);
      display.print("RH of Soil: ");
      display.print(soilmoisturepercent);
      display.print(" %");

      // display soil temperature
      display.setCursor(0, 20);
      display.print("Soil Temp: ");
      display.print(soiltemp);
      display.print(" ");
      display.cp437(true);
      display.write(167);
      display.print("C");

      // display air temperature
      display.setCursor(0, 35);
      display.print("Air Temp: ");
      display.print(temperature);
      display.print(" ");
      display.cp437(true);
      display.write(167);
      display.print("C");

      // display relative humidity of Air
      display.setCursor(0, 50);
      display.print("RH of Air: ");
      display.print(humidity);
      display.print(" %");

      display.display();
      delay(1500);
    }
    if (soilmoisturepercent.toInt() >= 0 && soilmoisturepercent.toInt() <= 30)
    {
      Serial.println("needs water, send notification");
      //send notification
      Blynk.logEvent("water_your_plants", "Lam on hay tuoi cay...") ;
      Serial.println("Motor is ON");
      Blynk.virtualWrite(V5, 255);
      delay(1000);

      // send data to Google Sheets
      sendata(soilmoisturepercent, soiltemp, temperature, humidity);
    }
    else if (soilmoisturepercent.toInt() > 30 && soilmoisturepercent.toInt() <= 100)
    {
      Serial.println("do am dat tot...");
      Serial.println("Motor is OFF");
      Blynk.virtualWrite(V5, 0);
      delay(1000);
    }
  }
}

void sendata(String soilmoisture, String soiltemp, String airtemp, String humidity){
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/macros/s/" + GAS_ID + "/exec?soilmoisture=" + soilmoisture + "&soiltemp=" + soiltemp + "&airtemp=" + airtemp + "&humidity=" + humidity;
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  Serial.println("request sent");
 
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
      }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
      Serial.println("esp8266/Arduino CI successful!");
  } else {
      Serial.println("esp8266/Arduino CI has failed");
  }
}

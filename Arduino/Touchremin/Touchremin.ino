#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "wifi.h"
#include <Adafruit_NeoPixel.h>

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

#define PIXELS 56
#define LOW_DIFF 0
#define HIGH_DIFF 50
#define filterWeight 0.5

const IPAddress outIP (192, 168, 0, 255);
const unsigned int outPort = 10000;
const unsigned int localPort = 10001;

IPAddress ip(192, 168, 0, 3);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);
IPAddress gateway(192, 168, 0, 1);

float lastProx[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int status = WL_IDLE_STATUS;
int val, sign = 1, firstPixelHue;
int startPixel = 0;
int sensors[12];
int alpha[12];
int leds [12][6] = {{54, 55}, {49, 50, 51, 52, 53}, {44, 45, 46, 47, 48}, {39, 40, 41, 42, 43}, {34, 35, 36, 37, 38}, {29, 30, 31, 32, 33}, {24, 25, 26, 27, 28}, {19, 20, 21, 22, 23}, {14, 15, 16, 17, 18}, {9, 10, 11, 12, 13}, {4, 5, 6, 7, 8}, {0, 1, 2, 3}};
int OSC[12];
boolean change = false;
long timed = millis();
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS; 

Adafruit_MPR121 cap[4] = {Adafruit_MPR121(), Adafruit_MPR121(), Adafruit_MPR121(), Adafruit_MPR121()};
Adafruit_NeoPixel strip(PIXELS, 10, NEO_GRB + NEO_KHZ800);
WiFiUDP UDP;

void setup() {
  strip.begin();
  cap[0].begin(0x5A);
  cap[0].setThresholds(2, 1);
  cap[1].begin(0x5B);
  cap[1].setThresholds(2, 1);
  cap[2].begin(0x5C);
  cap[2].setThresholds(2, 1);
  cap[3].begin(0x5D);
  cap[3].setThresholds(2, 1);
  Serial.begin(9600);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    //WiFi.config(ip, dns, gateway, subnet);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
  UDP.begin(localPort);
}


void loop() {
  readValues();
  runLeds();
  sendData();
}

int electrode(Adafruit_MPR121 cap, int input, int num) {
  int reading = cap.baselineData(input) - cap.filteredData(input);
  unsigned int prox = constrain(reading, LOW_DIFF, HIGH_DIFF);
  lastProx[num] = (filterWeight * lastProx[num]) + ((1 - filterWeight) * (float)prox);
  uint8_t thisOutput = (uint8_t)map(lastProx[num], LOW_DIFF, HIGH_DIFF, 0, 255);
  return thisOutput;
}

void sendData() {
  OSCMessage msg("/caps");
  msg.add(OSC[0]);
  msg.add(OSC[1]);
  msg.add(OSC[2]);
  msg.add(OSC[3]);
  msg.add(OSC[4]);
  msg.add(OSC[5]);
  msg.add(OSC[6]);
  msg.add(OSC[7]);
  msg.add(OSC[8]);
  msg.add(OSC[9]);
  msg.add(OSC[10]);
  msg.add(OSC[11]);
  UDP.beginPacket(outIP, outPort);
  msg.send(UDP);
  UDP.endPacket();

}

void runLeds() {
  for (int i = 0; i < 12; i++) {
    delay(1);
    if (sensors[i] > 100) {
      OSC[i] = 1;
      resetWait();
      alpha[i] += 5;
      if (alpha[i] > 255) {
        alpha[i] = 255;
      }
      for (int j = 0; j < sizeof(leds[i]); j++) {
        strip.setPixelColor(leds[i][j], strip.Color(alpha[i], alpha[i], alpha[i]));
      }
    }
    else {
      OSC[i] = 0;
      alpha[i] -= 5;
      if (alpha[i] < 0) {
        alpha[i] = 0;
      }
      for (int j = 0; j < sizeof(leds[i]); j++) {
        strip.setPixelColor(leds[i][j], strip.Color(alpha[i], alpha[i], alpha[i]));
      }
    }
  }
  if (millis() - timed > 10000) {
    val += 5 * sign;
    if (val == 255 || val == 70 && sign == -1) {
      sign = -sign;
    }
    gradient(val);
  }
  delay(1);
  strip.show();
}

void readValues() {
  sensors[0] = electrode(cap[1], 11, 0);
  sensors[1] = electrode(cap[1], 6, 1);
  sensors[2] = electrode(cap[1], 0, 2);
  sensors[3] = electrode(cap[3], 11, 3);
  sensors[4] = electrode(cap[3], 5, 4);
  sensors[5] = electrode(cap[3], 0, 5);
  sensors[6] = electrode(cap[2], 11, 6);
  sensors[7] = electrode(cap[2], 6, 7);
  sensors[8] = electrode(cap[2], 0, 8);
  sensors[9] = electrode(cap[0], 11, 9);
  sensors[10] = electrode(cap[0], 5, 10);
  sensors[11] = electrode(cap[0], 0, 11);
}

void resetWait() {
  timed = millis();
  val = 0;
  sign = 1;
}

void gradient(int alpha) {
  int sp = startPixel;
  for ( int i = 0; i < PIXELS / 2; i++ ) {
    strip.setPixelColor(sp, 255 - i * 2 , i * 3, 0 );
    if ( sp == PIXELS )
      sp = 0;
    else
      sp++;
  }
  for ( int i = PIXELS / 2; i >= 0 ; i-- ) {
    strip.setPixelColor(sp, 255 - i * 2 , i * 3, 0);
    if ( sp == PIXELS )
      sp = 0;
    else
      sp++;
  }
  startPixel++;
  if ( startPixel == PIXELS )
    startPixel = 0;
  strip.setBrightness(alpha);
}


void printWifiData() {
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

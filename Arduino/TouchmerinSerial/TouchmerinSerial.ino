#include <AP_Sync.h>
#include <Adafruit_NeoPixel.h>
#include <MPR121.h>
#include <Wire.h>
#include  "Compiler_Errors.h"

#define PIXELS 46
#define LOW_DIFF 0
#define HIGH_DIFF 50
#define filterWeight 0.3

float lastProx = 0;
int sensors[8];
int leds [8][6] = {{43, 44, 45, 46, 0, 1}, {37 , 38, 39, 40, 41, 42}, {31, 32, 33, 34, 35, 36}, {25, 26, 27, 28, 29, 30}, {19, 20, 21, 22, 23, 24}, {13, 14, 15, 16, 17, 18}, {7, 8, 9, 10, 11, 12}, {2, 3, 4, 5, 6}};
int alpha[8];
long timed = millis();
int val, sign = 1, firstPixelHue;
int startPixel = 0;

Adafruit_NeoPixel strip(PIXELS, 7, NEO_GRB + NEO_KHZ800);
AP_Sync streamer(Serial);

void setup() {
  strip.begin();
  Serial.begin(9600);
  if (!MPR121.begin(0x5C)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }
  MPR121.setRegister(MPR121_NHDF, 0x01);
  MPR121.setRegister(MPR121_FDLF, 0x3F);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    sensors[i] = electrode(i);
    if (sensors[i] > 0 && sensors[i] < 150) {
      resetWait();
      if (alpha[i] >= 20) {
        alpha[i] += 10;
      }
      else {
        alpha[i] += 5;
      }
      alpha[i] += 5;
      if (alpha[i] > 100) {
        alpha[i] = 100;
      }
      for (int j = 0; j < sizeof(leds[i]); j++) {
        strip.setPixelColor(leds[i][j], strip.Color(alpha[i], alpha[i], alpha[i]));
      }
    }
    else if (sensors[i] > 150) {
      resetWait();
      alpha[i] += 20;
      if (alpha[i] > 255) {
        alpha[i] = 255;
      }
      for (int j = 0; j < sizeof(leds[i]); j++) {
        strip.setPixelColor(leds[i][j], strip.Color(alpha[i], alpha[i], alpha[i]));
      }
    }
    else {
      if (alpha[i] > 20) {
        alpha[i] -= 20;
      }
      else {
        alpha[i] -= 2;
      }
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
  for (int i = 0; i < 8; i++){
      streamer.sync(("data"+String(i)), alpha[i]);
  }
}

int electrode(int input) {
  MPR121.updateAll();
  int reading = MPR121.getBaselineData(input) - MPR121.getFilteredData(input);
  unsigned int prox = constrain(reading, LOW_DIFF, HIGH_DIFF);
  lastProx = (filterWeight * lastProx) + ((1 - filterWeight) * (float)prox);
  uint8_t thisOutput = (uint8_t)map(lastProx, LOW_DIFF, HIGH_DIFF, 0, 255);
  return thisOutput;
}

void rainbow(int brightness) {
  if (firstPixelHue == 5 * 65536) {
    firstPixelHue = 0;
  }
  firstPixelHue += 256;
  for (int i = 0; i < strip.numPixels(); i++) {
    int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
  }
  strip.setBrightness(brightness);
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

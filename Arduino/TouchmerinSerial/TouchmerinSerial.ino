#include <Adafruit_NeoPixel.h>

#define PIXELS 177


float lastProx = 0;
int sensors[8];
int leds [12][6] = {{43, 44, 45, 46, 0, 1}, {37 , 38, 39, 40, 41, 42}, {31, 32, 33, 34, 35, 36}, {25, 26, 27, 28, 29, 30}, {19, 20, 21, 22, 23, 24}, {13, 14, 15, 16, 17, 18}, {7, 8, 9, 10, 11, 12}, {2, 3, 4, 5, 6}};
int alpha[8];
long timed = millis();
int val, sign = 1, firstPixelHue;
int startPixel = 0;

Adafruit_NeoPixel strip(PIXELS, 10, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  Serial.begin(9600);
}

void loop() {
  delay(10);
  gradient();
  strip.show();
}


void gradient() {
  int sp = startPixel;
  for ( int i = 0; i < PIXELS / 2; i++ ) {
    strip.setPixelColor(sp, i *2,i *2,i *2);
    if ( sp == PIXELS )
      sp = 0;
    else
      sp++;
  }
  for ( int i = PIXELS / 2; i >= 0 ; i-- ) {
    strip.setPixelColor(sp, i *2,i *2,i *2);
    if ( sp == PIXELS )
      sp = 0;
    else
      sp++;
  }
  startPixel++;
  if ( startPixel == PIXELS )
    startPixel = 0;
}

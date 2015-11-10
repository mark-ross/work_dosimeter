#include "Adafruit_NeoPixel.h"

#define LEDs 12
#define PIN 2

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDs, PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = 0xFF0000;
uint32_t off = 0x000000;
uint32_t green = 0x00FF00;
    

void setup() {
  strip.begin();
  strip.setBrightness(75);
  strip.show(); // Initialize all pixels to 'off'
}

void pixel_on(int pixel, uint32_t color){
  strip.setPixelColor(pixel, color);
}

void all_pixels(uint32_t color){
  for(int i = (LEDs-1); i>=0; i--)
  {
    pixel_on(i,color); 
  }
}

void loop() {
  /*
  // put your main code here, to run repeatedly:
  strip.setPixelColor(0, 255, 0, 255);
  strip.setPixelColor(1, 255, 255, 255);
  strip.setPixelColor(2, 0, 255, 255);
  strip.setPixelColor(3, 0, 0, 255);
  pixel_on(4,red);
  */

  all_pixels(green);
  
  strip.show();

}

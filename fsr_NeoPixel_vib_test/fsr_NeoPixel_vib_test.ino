#include "Adafruit_NeoPixel.h"
#include <elapsedMillis.h>

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
uint32_t off_light = 0x000000;
uint32_t green = 0x00FF00;
uint32_t blue = 0x0000FF;
uint32_t yellow = 0xFFFF00;

int fsr = 1;
int led = 13;
int vib = 0;

typedef enum {
  on,
  off,
} counting;

counting c = on;

void setup() {
  // put your setup code here, to run once:
  pinMode(fsr,INPUT);
  pinMode(led,OUTPUT);
  pinMode(vib, OUTPUT);

  strip.begin();
  strip.setBrightness(75);
  strip.show(); // Initialize all pixels to 'off'
}


void pixel_on(int pixel, uint32_t color){
  strip.setPixelColor(pixel, color);
}

void all_pixels(uint32_t color){
  for(int i = (LEDs-1); i>=0; i--)
    { pixel_on(i,color); }

  strip.show();
}

/*void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(fsr) == HIGH){
    //digitalWrite(led,HIGH);
    all_pixels(green);
    digitalWrite(vib,HIGH);
  }
  else {
    //digitalWrite(led,LOW);
    all_pixels(red);
    digitalWrite(vib,LOW);
  }

  strip.show();
}*/

//start timer
elapsedMillis c_dur = 0;
//start the button press timer
elapsedMillis c_button = 0;

void loop(){
      
  while(digitalRead(fsr) == HIGH){
    
    if(c_button >= 1000 && c_button < 2000){//1 Second
      all_pixels(green);
      if(c == on){
        //show lights
        //pulse motor
      } else if(c == off){
        //show lights
        //pulse motor
      }
    } 
    else if( c_button >= 2000 && c_button < 3000){//2 seconds
      //show lights
      all_pixels(blue);
      //pulse motor
    }
    else if( c_button >= 3000 && c_button < 4000){//3 seconds
      //show lights
      all_pixels(red);
      //pulse motor
    }
    else if( c_button > 4000 ){
      c_button -= 4000; //reset back to 0-ish
    }
  }
  
  //after it's no longer being pressed
    if( c_button >= 1000 && c_button < 2000){ //1 Second
      if(c == on){ c = off; }  //turn off the counting
      else { c = on; }         //turn on the counting
    } 
    else if( c_button >= 2000 && c_button < 3000){
      //show = true; //sets the show variable to true;
    }
    else if( c_button >= 3000 && c_button < 4000){
      //time_worked = 0; // reset the counter
    }
  
  //time_worked += (c_dur/1000); //add actual seconds, not milliseconds
  }

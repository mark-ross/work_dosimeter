#include <elapsedMillis.h>
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
uint32_t off_light = 0x000000;
uint32_t green = 0x00FF00;
uint32_t blue = 0x0000FF;
uint32_t yellow = 0xFFFF00;

/******************************
 * Type definitions.
 ******************************/
typedef enum {
  sleep,
  interact,
} states;

typedef enum {
  on,
  off,
} counting;


/**********************************
 * Declaration of global variables!
 **********************************/
states current = interact; //beginning state
states next = current;  //state to go to
counting c = off;       //if we're tracking time

int time_worked = 144000000;    //create a timer for time worked

//define all the pins here
int fsr = 1; //force sensor
int vib = 0; //vibration motor
int neo = 2; //neo pixel thing


void pixel_on(int pixel, uint32_t color){
  strip.setPixelColor(pixel, color);
}

void all_pixels(uint32_t color){
  for(int i = (LEDs-1); i>=0; i--)
    { pixel_on(i,color); }
    strip.show();
}


class Motor{
 
  public:
    int timer;
    bool on_off;
    int vib;

    Motor(int p){
      //set all the initial variables
      timer = 0;
      on_off = true;
      this->vib = p;
    }
    
    ~Motor(){}
    
    void set_timer(int t)
      { this->timer = t; }

    void flip_on_off(){
      if(on_off) { on_off = false; } 
      else { on_off = true; }
    }

    void check(int new_time,int thresh){
      if(new_time > this->timer){
        // if there has been a threshold of difference
        if((new_time - this->timer) >= thresh){
          this->flip_on_off();
          this->set_timer(new_time);
        }
      }
      else {
        this->on_off = false;
        this->set_timer(0);
      }
    }

    void motor_on()  { digitalWrite(this->vib, HIGH); }
    void motor_off() { digitalWrite(this->vib,LOW); }

    void react(){
      if(this->on_off) { motor_on(); } 
      else { motor_off(); }
    }
};




//This is our parent class.
class Engine{
  public:  // make sure the functions are public!
    virtual void event_handling() = 0;
    virtual void react() = 0;
};

//This class handles all the sleep functionality.
//Woo!
class Sleep : public Engine {

  public:
    void event_handling(){
      /*
       * Pseudo Code
       */
       if(c == off){ //if not counting
        //attach an interrupt so it will wake up
        //send arduino to sleep
    
        //change state to interact
        next = interact;
       }
       else{ //if counting
        //attach an interrupt so it will wake up
        //send arduino to sleep, based on counter
        //detach interrupt so the counting is hijacked
        time_worked += 8; //assuming the arduino is
                          //set to sleep for 8 seconds      
       }
    }

    void react(){
      //There isn't anything to show here...
    }

    void wake_up(){
      //This should be the LED show that let's
      //the user know that everything is awake.
    }

};


class Interact : public Engine {
  public:
    //set a boolean named show
    int show;
    elapsedMillis c_button;
    elapsedMillis c_dur;
    elapsedMillis down_time;
    Motor* haptic = new Motor(vib);

    Interact(){ 
      show = 0; //neutral
      //2 = start timer
      //3 = Show results
      //3 = reset master timer
    }
    
    ~Interact() {}
    
    void event_handling(){
      //start timer
      c_dur = 0;
      //start the button press timer
      c_button = 0;
      
      while(digitalRead(fsr) == HIGH){
        down_time = 0;
        if(c_button < 1000){
          show = 1;
          //show lights
          all_pixels(blue);
          //pulse motor
          haptic->check(c_button,100);
          haptic->react();
        }
        if(c_button >= 1000 && c_button < 4000){
          show = 2;
          //show lights
          all_pixels(green);
          //pusle motor
          haptic->check(c_button,200);
          haptic->react();
        } 
        if( c_button >= 4000 && c_button < 5000){
          show = 3;
          //show lights
          all_pixels(red);
          //pulse motor
          haptic->check(c_button,333);
          haptic->react();
          
        }
        if( c_button > 5000 ){
          c_button -= 5000; //reset back to 0-ish
        }
      }
      haptic->motor_off();
      time_worked += (c_dur/1000); //add actual seconds, not milliseconds
    }
    
    void react(){
      if(show == 0){
        if(down_time >= 4000){
          time_worked += down_time;
          //change state to sleep
        }
      }
      if(show == 1){ //if we are showing
        int th = ((time_worked/1000)/3600);
        int for_one = 40/12;
        int num_pix = th/for_one;
        
        if(th < 35){
          all_pixels(green);
        } else if (th >=35 && th < 40){
          all_pixels(yellow);
        } else {
          all_pixels(red);
        }
        delay(3000);
        all_pixels(off_light);
        time_worked += 3000;
        show = 0;
      }
      else if(show == 2){
        all_pixels(off_light);
        if(c == on){
          c = off;
        } else {
          c = on;
        }
      }
      else if(show == 3){
        all_pixels(off_light);
        time_worked = 0; //reset the master timer
      }
    }
};


Engine *engine = new Interact();

//Function to determine states
void change(){
  if(next != current){  // if we need to go somewhere
    delete engine;  // delete the current state
    
    switch(next) {  // based on what next is...
      
      case sleep:  // if it's start...
        engine = new Sleep();  // set engine to a new Start
        //Serial.write("Sleep State\n");
        break;     // exit the loop
        
      case interact:  // if it's interrupted
        engine = new Interact();  // set engine to a new Interrupted
        //Serial.write("interact State\n");
        break;     // exit the loop
    }
    current = next;
  }
}


void setup(){

  pinMode(vib,OUTPUT);
  pinMode(fsr,INPUT);
  pinMode(neo,OUTPUT);
  
  //perform any setup here.
  strip.begin();
  strip.setBrightness(75);
  strip.show(); // Initialize all pixels to 'off'

  all_pixels(green);
  delay(1000);
  all_pixels(off_light);
  strip.show();
}

void loop() {
  engine->event_handling();  // process events
  engine->react();   // React to the events
  change();  // handle any change in states
}

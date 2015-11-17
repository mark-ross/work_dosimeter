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

int time_worked = 0;    //create a timer for time worked

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
  private:
    int timer;
    bool on_off;
  
  public:

    Motor(){
      //set all the initial variables
      timer = 0;
      on_off = false;
    }
    
    ~Motor(){
      //nothing to delete. . .  
    }

    bool is_on(){
      //function to access variables!
      return on_off;
    }

    void flip_on_off(){
      if(this->on_off == false){
        this->on_off = true;
      }
      else {
        this->on_off = false;
      }
    }

    int current_time(){
      return timer;
    }

    void set_timer(int t){
      this->timer = t;
    }

    void flip_motor(){
      if(this->is_on()){  //if the motor is on. . .
        this->motor_off();  //turn it off
        this->flip_on_off();
      }
      else{  //if the motor is off
        this->motor_on();  //turn it on
        this->flip_on_off();
      }
    }

    void check_motor(int new_time, int thresh){
      if(new_time > this->current_time()){
        // if there has been a 500 millisec difference
        if((new_time - this->current_time()) >= thresh){
          this->flip_motor();
          this->set_timer(new_time);
        }
      }
      else {
        this->set_timer(new_time);
        this->motor_off();
      }
    }
  
    void motor_on(){ //turn the motor on
      digitalWrite(vib, HIGH);
    }
    void motor_off(){ //turn the motor off
      digitalWrite(vib,LOW);
    }
}haptic;




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

    Interact(){ 
      show = 0; //neutral
      //1 = start timer
      //2 = Show results
      //3 = reset master timer
    }
    
    ~Interact() {}
    
    void event_handling(){
      //start timer
      c_dur = 0;
      //start the button press timer
      c_button = 0;

      if(digitalRead(fsr) == LOW){
        all_pixels(off_light);
      }
      
      while(digitalRead(fsr) == HIGH){

        if(c_button < 1000){
          //show lights
          all_pixels(blue);
          //pulse motor
          haptic.check_motor(c_button,333);
        }
        if(c_button >= 1000 && c_button < 4000){
          //show lights
          all_pixels(green);
          //pusle motor
          haptic.check_motor(c_button,250);
        } 

        if( c_button >= 4000 && c_button < 5000){
          //show lights
          all_pixels(red);
          //pulse motor
          haptic.check_motor(c_button,100);
        }
        if( c_button > 5000 ){
          c_button -= 5000; //reset back to 0-ish
          //all_pixels(yellow);
        }
      }

      //after it's no longer being pressed
        if( c_button >= 1000 && c_button < 2000){ //1 Second
          if(c == on){ c = off; }  //turn off the counting
          else { c = on; }         //turn on the counting
        } 
        else if( c_button >= 2000 && c_button < 3000){
          show = true; //sets the show variable to true;
        }
        else if( c_button >= 3000 && c_button < 4000){
          time_worked = 0; // reset the counter
        }

      time_worked += (c_dur/1000); //add actual seconds, not milliseconds
    }
    void react(){
      if(show == 1){
        if(c == on){
          c = off;
        } else {
          c = on;
        }
      }
      else if(show == 2){ //if we are showing

        // convert millisecs to hours
        int th = ((time_worked/1000)/3600);
        
        if(th < 40){
          //show green
        }
        else if(th > 45){
          //show red
          //buzz  
        }
        else {
          //show blue
          //pulse
        }
      }
      else if(show == 3){
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
  //perform any setup here.
  strip.begin();
  strip.setBrightness(75);
  strip.show(); // Initialize all pixels to 'off'

  all_pixels(green);
  delay(1000);
  all_pixels(off_light);
  strip.show();
  //Serial.begin(9600);
}

void loop() {
  engine->event_handling();  // process events
  engine->react();   // React to the events
  change();  // handle any change in states
}

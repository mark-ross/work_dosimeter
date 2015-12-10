#include <elapsedMillis.h>
#include "Adafruit_NeoPixel.h"
#include <LowPower.h>


#define LEDs 12
#define PIN 5

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
  sleeper,
  interact,
  empty,
} states;

typedef enum {
  on,
  off,
} counting;


/**********************************
 * Declaration of global variables!
 **********************************/
states current = empty; //beginning state
states next = sleeper;  //state to go to
counting c = off;       //if we're tracking time

//uint32_t time_worked = 140000000;    //create a timer for time worked
uint32_t time_worked = 0;    //create a timer for time worked

//define all the pins here
int fsr = 2; //force sensor
int vib = 9; //vibration motor
int neo = 5; //neo pixel thing


void pixel_on(int pixel, uint32_t color){
  strip.setPixelColor(pixel, color);
  strip.show();
}

void pixel_off(int pixel){
  strip.setPixelColor(pixel, off_light);
  strip.show();
}

void all_pixels(uint32_t color){
  for(int i = (LEDs-1); i>=0; i--)
    { pixel_on(i,color); }
    strip.show();
}


//make this a global function for 
//Arduino reasons.
void wake_up(){
  //on interrupt, set the state to interact mode
  next = interact;
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
class Sleeper : public Engine {

  public:
    void event_handling(){
      /*
       * Pseudo Code
       */
      //set up an interrupt on pin 2
      attachInterrupt(0, wake_up, HIGH);
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }

    void react(){
      //There isn't anything to show here...
      if(c == on){
       detachInterrupt(0);
       time_worked += 8000; 
      }
    }
};


class Interact : public Engine {
  public:
    //set a boolean named show
    int show;
    elapsedMillis c_button;
    elapsedMillis c_dur;
    elapsedMillis down_time;
    elapsedMillis inactivity;
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

      //check for inactivity first!
      inactivity = 0;
      while(digitalRead(fsr) == LOW){
        if(inactivity >= 5000){
          time_worked += 5000;
          next = sleeper;
          break;
        }
      }
      
      while(digitalRead(fsr) == HIGH){
        if(c_button < 1000){
          show = 1;
          //show lights
          all_pixels(blue);
          //pulse motor
          haptic->check(c_button,100);
          haptic->react();
        }
        if(c_button >= 1000 && c_button < 3000){
          show = 2;
          //show lights
          all_pixels(green);
          //pusle motor
          haptic->check(c_button,200);
          haptic->react();
        } 
        if( c_button >= 3000 && c_button < 5000){
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
      //kill the motor pulses.
      haptic->motor_off();
      
      //zero out the button timer.
      c_button = 0;
    }
    
    void react(){
      if(show == 0){
        if(c){time_worked += c_dur;} //add duration of the loop
      }
      if(show == 1){ //if we are showing
        show = 0; //reset the switch

        Serial.print("Time worked: ");
        Serial.print(time_worked/1000/60/60);
        Serial.println(" hours");
        
        //convert the millisecs to hours
        int th = (((time_worked/1000)/60)/60);

        //set the threshholds
        // <35 hours = good,
        // 35 - 40 hours = meh,
        // 40+ = death
        if(th < 35){
          all_pixels(green);
        } else if (th >= 35 && th < 40){
          all_pixels(yellow);
        } else {
          all_pixels(red);
        }
        delay(3000); //show the lights for 3 seconds
        all_pixels(off_light); //turn off the ring
        
        //if the person is working. . .
        if(c){ time_worked += 3000; } //add the time to total
 
      }
      else if(show == 2){
        show = 0; //reset the switch
        if(c == on){ //if he/she was counting
          c = off;  //turn counting off
          all_pixels(yellow);  //turn on all the lights
          
          //then one by one turn them off
          for(int i = 11; i >= 0; i--)
          {
            pixel_off(i);
            delay(150);
          }
        } else { //otherwise
          c = on;  //turn counting on
          all_pixels(off_light);  //turn all the pixels off
          
          //then one by one turn them on
          for(int i = 0; i <= 11; i++)
          {
            pixel_on(i,yellow);
            delay(150);
          }
          delay(500); //give a final show of the lights
          all_pixels(off_light);  //turn them all off
        }
      }
      else if(show == 3){
        show = 0; //reset the switch
        
        all_pixels(off_light);
        
        time_worked = 0; //reset the master timer

        all_pixels(red);  //turn on all the lights
        
        //then one by one turn them off
        for(int i = 11; i >= 0; i--)
        {
          pixel_off(i);
          delay(300);
          strip.show();
        }

        all_pixels(red);
        delay(200);
        all_pixels(off_light);
        delay(200);
        all_pixels(red);
        delay(200);
        all_pixels(off_light);
        delay(200);
        all_pixels(red);
        delay(200);
        all_pixels(off_light);
      }
    }
};


Engine *engine = new Sleeper();

//Function to determine states
void change(){
  if(next != current){  // if we need to go somewhere
    delete engine;  // delete the current state
    
    switch(next) {  // based on what next is...
      
      case sleeper:  // if it's start...
        engine = new Sleeper();  // set engine to a new Start
        Serial.write("Sleep State\n");
        delay(100); //to let it actually write
        break;     // exit the loop
        
      case interact:  // if it's interrupted
        engine = new Interact();  // set engine to a new Interrupted
        Serial.write("interact State\n");
        delay(100); //to let it actually write
        break;     // exit the loop
    }
    current = next;
  }
}


void setup(){

  Serial.begin(9600);

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
  change();  // handle any change in states
  engine->event_handling();  // process events
  engine->react();   // React to the events
}



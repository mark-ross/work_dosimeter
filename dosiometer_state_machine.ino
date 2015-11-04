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
states current = sleep; //beginning state
states next = current;  //state to go to
counting c = off;       //if we're tracking time

int time_worked = 0;    //create a timer for time worked

//define all the pins here
int fsr = 1; //force sensor
int vib = 0; //vibration motor
int neo = 2; //neo pixel thing


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
    bool show = false;
    
    void event_handling(){
      //start timer
      int sub_counter = 0;
      
      while(digitalRead(fsr) == HIGH){
        //update timer
        //A:LKDJF:LAKJDF
        
        if(sub_counter >= 1000 && sub_counter < 2000){//1 Second
          if(c == on){
            //show lights
            //pulse motor
          } else if(c == off){
            //show lights
            //pulse motor
          }
        } 
        else if(sub_counter >= 2000 && sub_counter < 3000){//2 seconds
          //show lights
          //pulse motor
        }
        else if(sub_counter >= 3000 && sub_counter < 4000){//3 seconds
          //show lights
          //pulse motor
        }   
      }

      //after it's no longer being pressed
        if(sub_counter >= 1000 && sub_counter < 2000){ //1 Second
          if(c == on){ c = off; }  //turn off the counting
          else { c = on; }         //turn on the counting
        } 
        else if(sub_counter >= 2000 && sub_counter < 3000){
          show = true; //sets the show variable to true;
        }
        else if(sub_counter >= 3000 && sub_counter < 4000){
          time_worked = 0; // reset the counter
        }
    }
    void react(){
      if(show){ //if we are showing
        if(time_worked < 40){
          //show green
        }
        else if(time_worked > 45){
          //show red
          //buzz  
        }
        else {
          //show blue
          //pulse
        }
      }
    }
};


/*
 * This is where things get tricky.
 * Because of C++ inheritance, we can
 * do some awesome things.
 * 
 * We start by creating an instance of
 * the parent class, Engine.  This variable
 * is also a pointer.  Basically, we're creating
 * a changeable variable that will point at
 * whichever new state we send it to.
 * 
 * For now, we set it to Start()
 */
Engine *engine = new Sleep();


//Function to determine states
void change(){
  if(next != current){  // if we need to go somewhere
    
    delete engine;  // delete the current state
    
    switch(next) {  // based on what next is...
      
      case sleep:  // if it's start...
        engine = new Sleep();  // set engine to a new Start
        Serial.write("Sleep State\n");
        break;     // exit the loop
        
      case interact:  // if it's interrupted
        engine = new Interact();  // set engine to a new Interrupted
        Serial.write("interact State\n");
        break;     // exit the loop
    }
    current = next;
  }
}


void setup(){
  //perform any setup here.

  Serial.begin(9600);
}

void loop() {
  engine->event_handling();  // process events
  engine->react();   // React to the events
  change();  // handle any change in states
 
}

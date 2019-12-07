#include <elapsedMillis.h>

/*
This should get you close...
The routine looks at the state of 2 sensors, and the order of activation sets the direction of travel.
Handles a partial trigger (one sensor activated and then deactivated, say by a local switching move...
*/

enum { Waiting, TriggerLeft, GoingLeft, TriggerRight, GoingRight };

int getDOT(boolean Left, boolean Right ) {
    static int state = Waiting;
    
    switch (state) {
    case Waiting:
                  if (Left && !Right) { state = TriggerRight; }
                  if (!Left && Right) { state = TriggerLeft; }
                  break;
    
    case TriggerRight:
                  if (!Left) { state = Waiting; } // false alarm, didn't cross sensor boundry...
                  if (Right) { state = GoingRight; }
                  break;
                  
    case TriggerLeft:
                  if (!Right) { state = Waiting; } // false alarm...
                  if (Left) { state = GoingLeft; }
                  break;
    
    case GoingRight:
    case GoingLeft:
                  // wait until both sensors drop out...
                  // could also add a time delay ir tie into a block occupancy / current detector
                  // so dir of travel is remembered...
                  if (!Left && !Right) { state = Waiting; }
                  break;
    
    default:
                  state = Waiting;
    }
    return state;
}

boolean Left, Right;
elapsedMillis Ltimer, Rtimer;
#define HOLDTIME 2000 // 2 seconds hold time...

#define LeftSensor 10
#define RightSensor 11
#define LeftTrafficLamp 12
#define RightTrafficLamp 13

void setup() {
    pinMode(LeftSensor,       INPUT_PULLUP);
    pinMode(RightSensor,      INPUT_PULLUP);
    pinMode(LeftTrafficLamp,  OUTPUT);
    pinMode(RightTrafficLamp, OUTPUT);
    Left = 0;
    Right = 0;
}

void loop() {

    boolean testLeft  = digitalRead(LeftSensor);
    boolean testRight = digitalRead(RightSensor);
    
    if (testLeft) {
      Ltimer = 0; // reset hold timer because we got another detection...
      Left = 1;
    } else if (Ltimer > HOLDTIME) {
      Left = 0;
    }
    
    if (testRight) {
      Rtimer = 0;
      Right = 1;
    } else if (Ltimer > HOLDTIME) {
      Right = 0;
    }
    
    int DOT = getDOT(Left, Right);
    
    // do something with "state" variable..., which will have one of the values in the enum above...
    if (DOT == GoingLeft) {
        digitalWrite(LeftTrafficLamp, HIGH);
        digitalWrite(RightTrafficLamp, LOW);
    } else if (DOT == GoingRight) {
        digitalWrite(LeftTrafficLamp, LOW);
        digitalWrite(RightTrafficLamp, HIGH);
    } else {
        digitalWrite(LeftTrafficLamp, LOW);
        digitalWrite(RightTrafficLamp, LOW);
    }
    // ...
}

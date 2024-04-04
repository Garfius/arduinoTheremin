#include <SimpleTimer.h>

#define desiredTriggerTime 30
#define trig1Pin 12
#define echo1Pin 2
#define echo1int 0// el corresponent al echo

#define trig2Pin 13
#define echo2Pin 3
#define echo2int 1// el corresponent al echo

SimpleTimer timer;

volatile int triggerState = 0;                  // Count down counter to trigger pulse time
volatile long triggerEnd = 0;

volatile long echo1start = 0;                         // Records start of echo pulse 
volatile long echo1end = 0;                           // Records end of echo pulse
volatile long echo1duration = 0;                      // Duration - difference between end and start

volatile long echo2start = 0;                         // Records start of echo pulse 
volatile long echo2end = 0;                           // Records end of echo pulse
volatile long echo2duration = 0;                      // Duration - difference between end and start

void setup()
{
  pinMode(trig1Pin, OUTPUT);                           // Trigger pin set to output
  pinMode(echo1Pin, INPUT);                            // Echo pin set to input

  pinMode(trig2Pin, OUTPUT);                           // Trigger pin set to output
  pinMode(echo2Pin, INPUT);                            // Echo pin set to input
  timer.setInterval(250, printDist);
  timer.setInterval(250, trigger_pulse);
  //Timer1.initialize(TIMER_US);                        // Initialise timer 1
  //Timer1.attachInterrupt(timerIsr);                 // Attach interrupt to the timer service routine 
  attachInterrupt(echo1int, echo1interrupt, CHANGE);  // Attach interrupt to the sensor echo input
  attachInterrupt(echo2int, echo2interrupt, CHANGE);  // Attach interrupt to the sensor echo input
  Serial.begin(115200);                                // Initialise the serial monitor output
}

// ----------------------------------
// loop() Runs continuously in a loop.
// This is the background routine where most of the processing usualy takes place.
// Non time critical tasks should be run from here.
// ----------------------------------
void loop()
{
  if (triggerState == 0) {
    timer.run();
  }else{
    if (triggerEnd <= micros()) {
      digitalWrite(trig1Pin, LOW);               // Set the trigger output low
      digitalWrite(trig2Pin, LOW);               // Set the trigger output low
      triggerState = 0;
    }
  }
  
}
void printDist() {
  Serial.print(echo1duration);               // Print the distance in centimeters
  Serial.print(";");
  Serial.println(echo2duration);
}

// --------------------------
// trigger_pulse() called every 50 uS to schedule trigger pulses.
// Generates a pulse one timer tick long.
// Minimum trigger pulse width for the HC-SR04 is 10 us. This system
// delivers a 50 uS pulse.
// --------------------------
void trigger_pulse()
{
  if (triggerState == 0) {
    digitalWrite(trig1Pin, HIGH);              // Set the trigger output high
    digitalWrite(trig2Pin, HIGH);              // Set the trigger output high
    triggerEnd = micros()+desiredTriggerTime;
    triggerState = 1;
  }
}

// --------------------------
// echo_interrupt() External interrupt from HC-SR04 echo signal. 
// Called every time the echo signal changes state.
//
// Note: this routine does not handle the case where the timer
//       counter overflows which will result in the occassional error.
// --------------------------
void echo1interrupt()
{
  switch (digitalRead(echo1Pin))                     // Test to see if the signal is high or low
  {
  case HIGH:                                      // High so must be the start of the echo pulse
    echo1end = 0;                                 // Clear the end time
    echo1start = micros();                        // Save the start time
    break;

  case LOW:                                       // Low so must be the end of hte echo pulse
    echo1end = micros();                          // Save the end time
    echo1duration = echo1end - echo1start;        // Calculate the pulse duration
    break;
  }
}

void echo2interrupt()
{
  switch (digitalRead(echo2Pin))                     // Test to see if the signal is high or low
  {
  case HIGH:                                      // High so must be the start of the echo pulse
    echo2end = 0;                                 // Clear the end time
    echo2start = micros();                        // Save the start time
    break;

  case LOW:                                       // Low so must be the end of hte echo pulse
    echo2end = micros();                          // Save the end time
    echo2duration = echo2end - echo2start;        // Calculate the pulse duration
    break;
  }
}

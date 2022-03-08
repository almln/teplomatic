// https://www.makerguides.com/a4988-stepper-motor-driver-arduino-tutorial/
#include <AccelStepper.h>

#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

const int BUTTON_CLOSE_PIN = 4;
const int BUTTON_OPEN_PIN = 5;
int MAX_X = 0;

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_CLOSE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_OPEN_PIN, INPUT_PULLUP);

  // Set the maximum speed in steps per second:
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(-400);
  // go backward and find zero position
  while (1) {
    bool is_button_pressed = digitalRead(BUTTON_CLOSE_PIN);
    if (is_button_pressed == LOW) {
      Serial.println("Close button pressed");
      stepper.setCurrentPosition(0);
      break;
    }   
    stepper.runSpeed();
  }

  // go forward and find max position
  stepper.setSpeed(400);
  while (1) {
    bool is_button_pressed = digitalRead(BUTTON_OPEN_PIN);
    if (is_button_pressed == LOW) {
      Serial.println("Open button pressed");
      MAX_X = stepper.currentPosition();
      break;
    }
    stepper.runSpeed();
  }

}
void loop() { 
  // TODO
  // call temperature sensor, calculate the new position by the formula
  int x = int(MAX_X / 2.0);
  Serial.print("go to x: ");
  Serial.println(x);
  stepper.runToNewPosition(x);
}

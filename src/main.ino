// https://www.makerguides.com/a4988-stepper-motor-driver-arduino-tutorial/
#include <AccelStepper.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

// stepper pins
#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

// temperature sensor pin
#define ONE_WIRE_BUS 7

// button pins
#define BUTTON_CLOSE_PIN 4
#define BUTTON_OPEN_PIN 5

// temperature limits
#define MIN_T 17
#define MAX_T 25

// Here will be stepper max position after the interval calibration
int MAX_X = 0;

// setup temperature sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature t_sensor(&oneWire);

// setup stepper
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

// perform a calibration of the operational range
void setup() {
  Serial.begin(9600);

  // init temperature sensor
  t_sensor.begin();

  // setup pin modes for the buttons
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
  // Send the command to get temperature readings 
  t_sensor.requestTemperatures();
  
  delay(1000);
  
  // read temperature
  float t = t_sensor.getTempCByIndex(0);

  // calculate position by the linear interpolation
  int x = int((t - MIN_T) / (MAX_T - MIN_T) * MAX_X);

  Serial.print("go to x: ");
  Serial.println(x);

  // actuate
  stepper.runToNewPosition(x);
}

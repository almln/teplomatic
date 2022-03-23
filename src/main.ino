// https://www.makerguides.com/a4988-stepper-motor-driver-arduino-tutorial/
#include <AccelStepper.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

// stepper pins
#define DIR_PIN 2
#define STEP_PIN 3
#define motorInterfaceType 1

// temperature sensor pin
#define ONE_WIRE_BUS 8

// button pins
#define BUTTON_CLOSE_PIN 4
#define BUTTON_OPEN_PIN 5

// temperature limits
#define MIN_T 17
#define MAX_T 25

// Speed value
#define SPEED 600

// Here will be stepper max position after the interval calibration
int MAX_X = 0;

// setup temperature sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature t_sensor(&oneWire);

// setup stepper
AccelStepper stepper = AccelStepper(motorInterfaceType, STEP_PIN, DIR_PIN);

// perform a calibration of the operational range
void setup() {
  Serial.begin(9600);

  // init temperature sensor
  t_sensor.begin();

  // setup pin modes for the buttons
  pinMode(BUTTON_CLOSE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_OPEN_PIN, INPUT_PULLUP);

  // Set the maximum speed in steps per second:
  stepper.setMaxSpeed(SPEED);
  stepper.setSpeed(SPEED);
  // go backward and find zero position
  while (1) {
    bool is_button_pressed = digitalRead(BUTTON_CLOSE_PIN);
    if (is_button_pressed == HIGH) {
      Serial.println("Close button pressed");
      stepper.setCurrentPosition(0);
      break;
    }   
    stepper.runSpeed();
  }

  // go forward and find max position
  stepper.setSpeed(-SPEED);
  while (1) {
    bool is_button_pressed = digitalRead(BUTTON_OPEN_PIN);
    if (is_button_pressed == HIGH) {
      Serial.println("Open button pressed");
      MAX_X = stepper.currentPosition();
      Serial.print("Max position: ");
      Serial.println(MAX_X);
      break;
    }
    stepper.runSpeed();
  }
}

void loop() {
  // Send the command to get temperature readings 
  float t = 85.0;
  while (t > 84 || t < -100) {
    t_sensor.requestTemperatures();
    delay(1000);
    t = round(t_sensor.getTempCByIndex(0));
    delay(1000);
    // read temperature
    Serial.print("temperature: ");
    Serial.println(t);
  }
  
  // limit the temperature
  t = constrain(t, MIN_T, MAX_T);

  // calculate position by the linear interpolation
  int x = int((t - MIN_T) / (MAX_T - MIN_T) * MAX_X);

  // debug print
  // Serial.print("go to position: ");
  // Serial.println(x);  

  // actuate
  stepper.moveTo(x);
  stepper.setSpeed(SPEED);
  while(stepper.currentPosition() != x) 
  {
    stepper.runSpeedToPosition();
  }
}

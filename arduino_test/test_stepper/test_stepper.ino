// Include the AccelStepper Library
#include <AccelStepper.h>

// Define step constant
#define MotorInterfaceType 8

int tick_1 =  409;
int new_number = 9;
int old_number = 9;
// Creates an instance
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
AccelStepper myStepper(MotorInterfaceType, 8, 10, 9, 11);


int finish_homing = 0; 
int Homing_sensor = 1;


void setup() {
  // set the maximum speed, acceleration factor,
  // initial speed and the target position
  myStepper.setMaxSpeed(1000.0);
  myStepper.setAcceleration(500.0);
  myStepper.moveTo(-10000000);

  pinMode(A0,INPUT);
  Serial.begin(115200);
}

void loop() {

//   Change direction once the motor reaches target position
    int read1 = digitalRead(A0);
    // Serial.println(read1);
    
  if(read1 == 0 && finish_homing == 0){
    finish_homing = 1;
    myStepper.setMaxSpeed(700.0);
    myStepper.setAcceleration(1500.0);
    myStepper.setCurrentPosition(0);
    myStepper.moveTo(0);
    }
  if(finish_homing == 1) {
    // Serial.print("current_state = ");
    // Serial.println(myStepper.currentPosition());
    
    if(new_number != old_number){
        int distant_to_move = new_number - old_number;
        Serial.print("distant_to_move  ");
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        Serial.println(distant_to_move);
        myStepper.moveTo(myStepper.currentPosition() - distant_to_move * tick_1);
        old_number = new_number;
      }
    }

  
  if (myStepper.distanceToGo() != 0){
      myStepper.run();
      // Serial.println(myStepper.StepReturn());
    }
  else {
    if (Serial.available() != 1){
      new_number = Serial.parseInt();
    }
    Serial.print("new_number = ");
    Serial.println(new_number);
    myStepper.disableOutputs();
  }
  
}

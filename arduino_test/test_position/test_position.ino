// Include the AccelStepper Library
#include <AccelStepper.h>
#include <ShiftRegister74HC595.h>
ShiftRegister74HC595<2> sr(2,3,4);  // (data pin, clock pin, latch pin)


// Define step constant
#define MotorInterfaceType 8

int tick_1 =  409;
uint8_t step;
uint8_t step_STOP = 0;
// Creates an instance
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
AccelStepper myStepper(MotorInterfaceType);

long position_to_go;

void setup() {
  // set the maximum speed, acceleration factor,
  // initial speed and the target position
  myStepper.setMaxSpeed(1700.0);
  myStepper.setAcceleration(250.0);
  Serial.begin(115200);
}

void loop() {

  if (myStepper.distanceToGo() != 0){
      myStepper.run();
      if (myStepper.StepReturn() != step){
        step = myStepper.StepReturn();
        sr.setNoUpdate(0,(step >> 0 & 1));
        sr.setNoUpdate(2,(step >> 1 & 1));
        sr.setNoUpdate(1,(step >> 2 & 1));
        sr.setNoUpdate(3,(step >> 3 & 1));

        sr.setNoUpdate(4,(step >> 0 & 1));
        sr.setNoUpdate(6,(step >> 1 & 1));
        sr.setNoUpdate(5,(step >> 2 & 1));
        sr.setNoUpdate(7,(step >> 3 & 1));

        sr.setNoUpdate(8,(step >> 0 & 1));
        sr.setNoUpdate(10,(step >> 1 & 1));
        sr.setNoUpdate(9,(step >> 2 & 1));
        sr.setNoUpdate(11,(step >> 3 & 1));

        sr.setNoUpdate(12,(step >> 0 & 1));
        sr.setNoUpdate(14,(step >> 1 & 1));
        sr.setNoUpdate(13,(step >> 2 & 1));
        sr.setNoUpdate(15,(step >> 3 & 1));
        sr.updateRegisters();
        //delay(10);
      }
      
    }
  else {
    if (Serial.available() != 1){
      position_to_go = Serial.parseInt();
      Serial.print("position_to_go = ");
      Serial.println(position_to_go);
      myStepper.moveTo(position_to_go);
    }
    sr.setAllLow();
  }
  
}

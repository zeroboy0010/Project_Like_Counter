// Include the AccelStepper Library
#include <Arduino.h>
#include <AccelStepper.h>
#include <ShiftRegister74HC595.h>

ShiftRegister74HC595<2> sr(3,2,4);  // (data pin, clock pin, latch pin)

// pin for shiftin
const uint8_t dataPin = 7;   /* Q7 */
const uint8_t clockPin = 6;  /* CP */
const uint8_t latchPin = 8;  /* PL */
const uint8_t numBits = 8;   /* Set to 8 * number of shift registers */

unsigned long last_millis;
// hal sensor
uint8_t array_bit[8] = {1,1,1,1,1,1,1,1};
uint8_t stepper1_hal = 1;
uint8_t stepper2_hal = 1;
uint8_t stepper3_hal = 1;
uint8_t stepper4_hal = 1;

// Define step constant
const uint16_t tick =  409;
// stepper1
uint8_t stepper1_new_number = 0;
uint8_t stepper1_old_number = 0;
uint8_t stepper1_finish_homing = 0; 
uint8_t stepper1_Homing_sensor = 1;
// stepper2
uint8_t stepper2_new_number = 0;
uint8_t stepper2_old_number = 0;
uint8_t stepper2_finish_homing = 0; 
uint8_t stepper2_Homing_sensor = 1;
// stepper3
uint8_t stepper3_new_number = 9;
uint8_t stepper3_old_number = 9;
uint8_t stepper3_finish_homing = 0; 
uint8_t stepper3_Homing_sensor = 1;
// stepper4
uint8_t stepper4_new_number = 9;
uint8_t stepper4_old_number = 9;
uint8_t stepper4_finish_homing = 0; 
uint8_t stepper4_Homing_sensor = 1;

// state shift out
#define MotorInterfaceType 8
uint8_t step1;
uint8_t step2;
uint8_t step3;
uint8_t step4;
// Creates an instance
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
AccelStepper myStepper1(MotorInterfaceType);
AccelStepper myStepper2(MotorInterfaceType);
AccelStepper myStepper3(MotorInterfaceType);
AccelStepper myStepper4(MotorInterfaceType);

long position_to_go_1;
long position_to_go_2;
long position_to_go_3;
long position_to_go_4;

int speed_ = 250;
int accel_ = 100;

void read_hal();

int firstDigit(int n) 
{ 
    // Remove last digit from number 
    // till only one digit is left 
    while (n >= 10)  
        n /= 10; 
      
    // return the first digit 
    return n; 
} 
  
// Find the last digit 
int lastDigit(int n) 
{ 
    // return the last digit 
    return (n % 10); 
} 

void setup() {
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  // set the maximum speed, acceleration factor,
  // initial speed and the target position
  myStepper1.setMaxSpeed(speed_);
  myStepper1.setAcceleration(accel_);
  myStepper1.moveTo(10000000);
  myStepper2.setMaxSpeed(speed_);
  myStepper2.setAcceleration(accel_);
  myStepper2.moveTo(10000000);
  myStepper3.setMaxSpeed(speed_);
  myStepper3.setAcceleration(accel_);
  myStepper3.moveTo(0);
  myStepper4.setMaxSpeed(speed_);
  myStepper4.setAcceleration(accel_);
  myStepper4.moveTo(0);


  Serial.begin(9600);
}

void loop() {
  while (millis() - last_millis >= 500){
    last_millis = millis();
    read_hal();
  }
// part read from shift in
  stepper1_hal = array_bit[7];
  stepper2_hal = array_bit[6];
  stepper3_hal = array_bit[5];
  stepper4_hal = array_bit[4];
//

  if(stepper1_hal == 0 && stepper1_finish_homing == 0){
    stepper1_finish_homing = 1;
    myStepper1.setMaxSpeed(700.0);
    myStepper1.setAcceleration(1500.0);
    myStepper1.setCurrentPosition(0);
    myStepper1.moveTo(0);
    }

  if(stepper2_hal == 0 && stepper2_finish_homing == 0){
    stepper2_finish_homing = 1;
    myStepper2.setMaxSpeed(700.0);
    myStepper2.setAcceleration(1500.0);
    myStepper2.setCurrentPosition(0);
    myStepper2.moveTo(0);
    }

  if(stepper3_hal == 0 && stepper3_finish_homing == 0){
    stepper3_finish_homing = 1;
    myStepper3.setMaxSpeed(700.0);
    myStepper3.setAcceleration(1500.0);
    myStepper3.setCurrentPosition(0);
    myStepper3.moveTo(0);
    }

  if(stepper4_hal == 0 && stepper4_finish_homing == 0){
    stepper4_finish_homing = 1;
    myStepper4.setMaxSpeed(700.0);
    myStepper4.setAcceleration(1500.0);
    myStepper4.setCurrentPosition(0);
    myStepper4.moveTo(0);
    }

// part move
  if(stepper1_finish_homing == 1) {
    // Serial.print("current_state = ");
    // Serial.println(myStepper.currentPosition());
    
    if(stepper1_new_number != stepper1_old_number){
        int distant_to_move = stepper1_new_number - stepper1_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        Serial.println(distant_to_move);
        myStepper1.moveTo(myStepper1.currentPosition() + distant_to_move * tick);
        stepper1_old_number = stepper1_new_number;
      }
    }
  if(stepper2_finish_homing == 1) {
    
    if(stepper2_new_number != stepper2_old_number){
        int distant_to_move = stepper2_new_number - stepper2_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        Serial.println(distant_to_move);
        myStepper2.moveTo(myStepper2.currentPosition() + distant_to_move * tick);
        stepper2_old_number = stepper2_new_number;
      }
    }
  if(stepper3_finish_homing == 1) {
    
    if(stepper3_new_number != stepper3_old_number){
        int distant_to_move = stepper3_new_number - stepper3_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        Serial.println(distant_to_move);
        myStepper3.moveTo(myStepper3.currentPosition() + distant_to_move * tick);
        stepper3_old_number = stepper3_new_number;
      }
    }
  if(stepper4_finish_homing == 1) {
    
    if(stepper4_new_number != stepper4_old_number){
        int distant_to_move = stepper4_new_number - stepper4_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        Serial.println(distant_to_move);
        myStepper4.moveTo(myStepper4.currentPosition() + distant_to_move * tick);
        stepper4_old_number = stepper4_new_number;
      }
    }

  if (myStepper1.distanceToGo() != 0){
      myStepper1.run();
      if (myStepper1.StepReturn() != step1){
        step1 = myStepper1.StepReturn();
        sr.setNoUpdate(4,(step1 >> 0 & 1));
        sr.setNoUpdate(6,(step1 >> 1 & 1));
        sr.setNoUpdate(5,(step1 >> 2 & 1));
        sr.setNoUpdate(7,(step1 >> 3 & 1));
        // sr.updateRegisters();
      } 
    }
  else {
    step1 = 0;
    sr.setNoUpdate(4, 0);
    sr.setNoUpdate(5, 0);
    sr.setNoUpdate(6, 0);
    sr.setNoUpdate(7, 0);
    // sr.updateRegisters();
  }

    if (myStepper4.distanceToGo() != 0){
      myStepper4.run();
      if (myStepper4.StepReturn() != step4){
        step4 = myStepper4.StepReturn();
        sr.setNoUpdate(0,(step4 >> 0 & 1));
        sr.setNoUpdate(2,(step4 >> 1 & 1));
        sr.setNoUpdate(1,(step4 >> 2 & 1));
        sr.setNoUpdate(3,(step4 >> 3 & 1));
        // sr.updateRegisters();
      } 
    }
  else {
    step4 = 0;
    sr.setNoUpdate(0, 0);
    sr.setNoUpdate(1, 0);
    sr.setNoUpdate(2, 0);
    sr.setNoUpdate(3, 0);
    // sr.updateRegisters();
  }

    if (myStepper3.distanceToGo() != 0){
      myStepper3.run();
      if (myStepper3.StepReturn() != step3){
        step3 = myStepper3.StepReturn();
        sr.setNoUpdate(12,(step3 >> 0 & 1));
        sr.setNoUpdate(14,(step3 >> 1 & 1));
        sr.setNoUpdate(13,(step3 >> 2 & 1));
        sr.setNoUpdate(15,(step3 >> 3 & 1));
        // sr.updateRegisters();
      } 
    }
  else {
    step3 = 0;
    sr.setNoUpdate(12,  0);
    sr.setNoUpdate(13, 0);
    sr.setNoUpdate(14,  0);
    sr.setNoUpdate(15, 0);
    // sr.updateRegisters();
  }

    if (myStepper2.distanceToGo() != 0){
      myStepper2.run();
      if (myStepper2.StepReturn() != step2){
        step2 = myStepper2.StepReturn();
        sr.setNoUpdate(8,(step2 >> 0 & 1));
        sr.setNoUpdate(10,(step2 >> 1 & 1));
        sr.setNoUpdate(9,(step2 >> 2 & 1));
        sr.setNoUpdate(11,(step2 >> 3 & 1));
        // sr.updateRegisters();
      } 
    }
  else {
    step2 = 0;
    sr.setNoUpdate(8, 0);
    sr.setNoUpdate(9, 0);
    sr.setNoUpdate(10, 0);
    sr.setNoUpdate(11, 0);
    // sr.updateRegisters();
  }

// not sure ::::
  sr.updateRegisters();
  // delayMicroseconds(100);

  if (stepper1_finish_homing  && stepper2_finish_homing ){
    if (Serial.available() != 1){
      int position_to_go = Serial.parseInt(); 
      stepper1_new_number = firstDigit(position_to_go);
      stepper2_new_number = lastDigit(position_to_go);
    }
    
  }
  
  if( false ) 
  {
    Serial.print(step1);

    Serial.print("\t");
    Serial.print(step2);
    Serial.print("\t");
    Serial.print(step3);
    Serial.print("\t");
    Serial.print(step4);
    Serial.println();
  }



}


void read_hal() {
  // Step 1: Sample
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);

  // Step 2: Shift
  Serial.print("Bits: ");
  for (int i = 0; i < numBits; i++) {
    int bit = digitalRead(dataPin);
    array_bit[i] = bit;
    if (bit == HIGH) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
    digitalWrite(clockPin, HIGH); // Shift out the next bit
    digitalWrite(clockPin, LOW);
  }
}
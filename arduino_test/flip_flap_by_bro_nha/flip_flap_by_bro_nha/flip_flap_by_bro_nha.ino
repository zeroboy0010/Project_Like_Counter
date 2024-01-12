
char Data;
// const int CW =1;
const int CCW =2;
const int STOP =3;

int Pin1 = 8;//IN1 is connected to 8 
int Pin2 = 9;//IN2 is connected to 9  
int Pin3 = 10;//IN3 is connected to 10 
int Pin4 = 11;//IN4 is connected to 11
int switchSTOP =2;//define input pin for STOP push button
int stopType=2;//1=normal stop, 2=hold stop (consumes power)

int speedFactor =1;//1=fastest, 2=slower or 3 more slower
               //  1     2     3    4      5     6     7     8     9     0  
long angles[] = {40*1, 40*2, 40*3, 40*4, 40*5, 40*6, 40*7, 40*8, 40*9, 40*10 };//angles of each push button



int correction_CW = 150;//watch video for details
int correction_CCW = 150;//watch video for details


int poleStep = 0; 
long stepVale =0;
const int SPR=64*64;
long goToAngle=0;
int activeButton=0;
int pole1[] ={0,0,0,0, 0,1,1,1, 0};//pole1, 8 step values
int pole2[] ={0,0,0,1, 1,1,0,0, 0};//pole2, 8 step values
int pole3[] ={0,1,1,1, 0,0,0,0, 0};//pole3, 8 step values
int pole4[] ={1,1,0,0, 0,0,0,1, 0};//pole4, 8 step values



int count=0;
int  dirStatus = STOP;// stores direction status 3= stop (do not change)

void setup() 
{ 
  
  Serial.begin(9600);

 pinMode(Pin1, OUTPUT);//define pin for ULN2003 in1 
 pinMode(Pin2, OUTPUT);//define pin for ULN2003 in2   
 pinMode(Pin3, OUTPUT);//define pin for ULN2003 in3   
 pinMode(Pin4, OUTPUT);//define pin for ULN2003 in4   
 
} //setup
 void loop() 
{ 

  stepVale = (SPR * goToAngle)/360 ;
  
  if(Serial.available()> 0){
    Data = Serial.read();
    
    if(Data == '0'){
      goToAngle = angles[0];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number 1");
    }
    else if(Data == '1'){
      goToAngle = angles[1];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number 2");
    }
    else if(Data == '2'){
      goToAngle = angles[2];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number 3");
    }
    else if(Data == '3'){
      goToAngle = angles[3];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number4");
    }
    else if(Data == '4'){
      goToAngle = angles[4];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number 5");
    }
    else if(Data == '5'){
      goToAngle = angles[5];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number 6");
    }
    else if(Data == '6'){
      goToAngle = angles[6];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number 7");
    }
    else if(Data == '7'){
      goToAngle = angles[7];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number 8");
    }
    else if(Data == '8'){
      goToAngle = angles[8];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number 9");
    }
    else if(Data == '9'){
      goToAngle = angles[9];
      dirStatus = CCW;
      count = 0;
      Serial.println("Number 0");
    }

  }


  
 if(dirStatus == CCW){ 
  
   poleStep++; 
   count++;   
   if(count+correction_CCW <= stepVale)
   {
    driveStepper(poleStep);      
   }else{
      stopMotor();  
   }
 }

 else{
  stopMotor();   
 }
 if(poleStep>7){ 
   poleStep=0;
 } 
 if(poleStep<0){ 
   poleStep=7; 
 } 
  delay(1);

}// loop

void driveStepper(int c)
{
    //Robojax.com Stepper Push button Any Angle STPB-5
     digitalWrite(Pin1, pole1[c]);  
     digitalWrite(Pin2, pole2[c]); 
     digitalWrite(Pin3, pole3[c]); 
     digitalWrite(Pin4, pole4[c]);
    
}//driveStepper ends here

void stopMotor()
{

 dirStatus = STOP;
 if( stopType == 2)
 {
  driveStepper(8);
 }
}//stopMotor()


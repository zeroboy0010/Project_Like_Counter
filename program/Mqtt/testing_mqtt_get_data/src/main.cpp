#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "esp_heap_caps.h"
#include <max7219.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include <AccelStepper.h>

#define Device_name "plc_1"
#define Device_name_like_cnt "plc_1/like"

MAX7219 max7219;


// #define MQTT_HOST IPAddress(192, 168, 1, 10)

#define MQTT_HOST "0.tcp.ap.ngrok.io"
#define MQTT_PORT 13872


AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
//
/* 1. Define the WiFi credentials */
String Input;
String Output;
String Password;
String SSID;
String PageID;
String Password_Read;
String SSID_Read;
String PageID_Read;

char WIFI_SSID[20];
char WIFI_PASSWORD[20];
char PAGE_ID[20];

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe(Device_name_like_cnt, 1);
  Serial.print("Subscribing at QoS 2, packetId: ");
  uint16_t packetIdPub2 = mqttClient.publish(Device_name, 2, true, PAGE_ID);
  Serial.print("Publishing at QoS 2, packetId: ");
  Serial.println(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  payload: ");
  Serial.println(payload);
  max7219.Clear();
  max7219.DisplayText(payload, 1);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}




bool Wifi_status = true;   //state to run wifi
bool taskCompleted = false;


// Replace with your network credentials
const char* ssid     = "Like_counter_demo";
const char* password = "12345678";
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
// Auxiliar variables to store the current output state
String LED_state = "off";
// Assign output variables to GPIO pins
const int output26 = LED_BUILTIN;

unsigned long dataMillis = 0;

int To_connect_wifi;
#define Input_Button 23

int writeStringToEEPROM(int addrOffset, const String &strToWrite);
int readStringFromEEPROM(int addrOffset, String *strToRead);

void connect();
void running_ap();

//freertos
// Define two tasks for Blink & AnalogRead.
void TaskMotor( void *pvParameters );
void TaskHalSensor  ( void *pvParameters );
void Task_Data  ( void *pvParameters );
void running_ap  ( void *pvParameters );
//
SemaphoreHandle_t homed_1;
SemaphoreHandle_t homed_2;
SemaphoreHandle_t homed_3;
SemaphoreHandle_t homed_4;
SemaphoreHandle_t homed_5;
SemaphoreHandle_t homed_6;
SemaphoreHandle_t homed_7;
QueueHandle_t like_num;

#define sleep_pin 15

#define step_7 19
#define step_6 18       
#define step_5 5
#define step_4 17
#define step_3 16
#define step_2 4
#define step_1 2

#define sensor_1 32
#define sensor_2 33
#define sensor_3 25
#define sensor_4 26
#define sensor_5 27
#define sensor_6 14
#define sensor_7 12

// put function declarations here:
#define MotorInterfaceType 1

int speed_ = 200;
int accel_ = 50;
int __accel = 150;
int __speed = 300;

//
int arr[7]; 
void extract_digit(int N) 
{ 
    // To store the digit 
    // of the number N 
    
    int i = 0; 
    int r; 
  
    // Till N becomes 0 
    while (N != 0) { 
  
        // Extract the last digit of N 
        r = N % 10; 
  
        // Put the digit in arr[] 
        arr[i] = r; 
        i++; 
  
        // Update N to N/10 to extract 
        // next last digit 
        N = N / 10; 
    } 
    while(i<7){
      arr[i] = 0;
      i++;
    }
} 

xTaskHandle taskHandle_delete;
  

void setup() {
  Serial.begin(9600);
  /////////////////////////////////
  EEPROM.begin(150);
  pinMode(Input_Button,INPUT_PULLUP);
  max7219.Begin();
  for (int i=0; i<50;i++){
    Serial.print(".");
    if (digitalRead(Input_Button) == 0){
      To_connect_wifi = 0;
      break;
    }
    else {
      To_connect_wifi = 1;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  
  Serial.println("---------------------");
  if(To_connect_wifi == 1 ){
    WiFi.onEvent(WiFiEvent);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    connect();
    vTaskDelay(pdMS_TO_TICKS(3000));
    /* Assign the api key (required) */

    homed_1 = xSemaphoreCreateBinary();
    homed_2 = xSemaphoreCreateBinary();
    homed_3 = xSemaphoreCreateBinary();
    homed_4 = xSemaphoreCreateBinary();
    homed_5 = xSemaphoreCreateBinary();
    homed_6 = xSemaphoreCreateBinary();
    homed_7 = xSemaphoreCreateBinary();

    like_num = xQueueCreate(2,sizeof(int));
    /////
    if (Wifi_status == 1){
      
      mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
      wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
      
      xTaskCreatePinnedToCore(
      Task_Data
      ,  "Task_Data" // A name just for humans
      ,  2048 * 10      // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
      ,  NULL // Task parameter which can modify the task behavior. This must be passed as pointer to void.
      ,  3  // Priority
      ,  NULL
      ,  0 // Task handle is not used here - simply pass NULL
      );
    }

  // xTaskCreatePinnedToCore(
  //   TaskMotor
  //   ,  "TaskMotor" // A name just for humans
  //   ,  2048 * 5       // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
  //   ,  NULL // Task parameter which can modify the task behavior. This must be passed as pointer to void.
  //   ,  5  // Priority
  //   ,  NULL
  //   , 1 // Task handle is not used here - simply pass NULL
  //   );
  // xTaskCreatePinnedToCore(
  //   TaskHalSensor
  //   ,  "TaskHalSensor" // A name just for humans
  //   ,  2048 * 4       // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
  //   ,  NULL // Task parameter which can modify the task behavior. This must be passed as pointer to void.
  //   ,  4  // Priority
  //   ,  &taskHandle_delete
  //   , 1 // Task handle is not used here - simply pass NULL
  //   );
  }
  else {
    xTaskCreatePinnedToCore(
    running_ap
    ,  "running_ap" // A name just for humans
    ,  2048 * 4       // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,  NULL // Task parameter which can modify the task behavior. This must be passed as pointer to void.
    ,  3  // Priority
    ,  NULL
    ,  0 // Task handle is not used here - simply pass NULL
    );
  }

}

void loop() {
  // put your main code here, to run repeatedly
}

void TaskHalSensor(void *pvParameters){
  uint8_t array_pin_hal[7] = {sensor_1,sensor_2,sensor_3,sensor_4,sensor_5,sensor_6,sensor_7};
  for(int i=0;i<=6;i++){
    pinMode(array_pin_hal[i],INPUT);
  }
  uint8_t array_bit[7] = {0,0,0,0,0,0,0};
  uint8_t old_array_bit[7] = {0,0,0,0,0,0,0};
  uint8_t hal_sensor_home[7] = {1,1,1,1,1,1,1};

  for(;;){
    for (int i = 0;i <= 6; i++){
      array_bit[i] = digitalRead(array_pin_hal[i]);
      Serial.print(array_bit[i]);
    }
    Serial.println();
    for(int i = 6;i>=0;i--){
      if((array_bit[i] < old_array_bit[i] ) && hal_sensor_home[i] == 1){
        hal_sensor_home[i] = 0;
        if(i == 6){
          xSemaphoreGive(homed_1);
        }
        else if(i == 5){
          xSemaphoreGive(homed_2);
        }
        else if(i == 4){
          xSemaphoreGive(homed_3);
        }
        else if(i == 3){
          xSemaphoreGive(homed_4);
        }
        else if(i == 2){
          xSemaphoreGive(homed_5);
        }
        else if(i == 1){
          xSemaphoreGive(homed_6);
        }
        else if(i == 0){
          xSemaphoreGive(homed_7);
        }
      }
      old_array_bit[i] = array_bit[i];
    }
    vTaskDelay(pdMS_TO_TICKS(300));

  }
  vTaskDelete(NULL);
}



// put function definitions here:
void TaskMotor(void *pvParameters)
{
  uint8_t tick = 205;
    // stepper1
  uint8_t stepper1_new_number = 6;
  uint8_t stepper1_old_number = 6;
  uint8_t stepper1_finish_homing = 0; 
  uint8_t stepper1_Homing_sensor = 1;
  // stepper2
  uint8_t stepper2_new_number = 5;
  uint8_t stepper2_old_number = 5;
  uint8_t stepper2_finish_homing = 0; 
  uint8_t stepper2_Homing_sensor = 1;
  // stepper3
  uint8_t stepper3_new_number = 1;
  uint8_t stepper3_old_number = 1;
  uint8_t stepper3_finish_homing = 0; 
  uint8_t stepper3_Homing_sensor = 1;
  // stepper4
  uint8_t stepper4_new_number = 7;
  uint8_t stepper4_old_number = 7;
  uint8_t stepper4_finish_homing = 0; 
  uint8_t stepper4_Homing_sensor = 1;
    // stepper5
  uint8_t stepper5_new_number = 3;
  uint8_t stepper5_old_number = 3;
  uint8_t stepper5_finish_homing = 0; 
  uint8_t stepper5_Homing_sensor = 1;
    // stepper6
  uint8_t stepper6_new_number = 1;
  uint8_t stepper6_old_number = 1;
  uint8_t stepper6_finish_homing = 0; 
  uint8_t stepper6_Homing_sensor = 1;
    // stepper7
  uint8_t stepper7_new_number = 1;
  uint8_t stepper7_old_number = 1;
  uint8_t stepper7_finish_homing = 0; 
  uint8_t stepper7_Homing_sensor = 1;

  AccelStepper myStepper1(MotorInterfaceType,  step_1);
  AccelStepper myStepper2(MotorInterfaceType,  step_2);
  AccelStepper myStepper3(MotorInterfaceType,  step_3);
  AccelStepper myStepper4(MotorInterfaceType,  step_4);
  AccelStepper myStepper5(MotorInterfaceType,  step_5);
  AccelStepper myStepper6(MotorInterfaceType,  step_6);
  AccelStepper myStepper7(MotorInterfaceType,  step_7);
  // 
  pinMode(sleep_pin, OUTPUT);
  myStepper1.setMaxSpeed(speed_);
  myStepper1.setAcceleration(accel_);
  myStepper1.moveTo(10000000);
  myStepper2.setMaxSpeed(speed_);
  myStepper2.setAcceleration(accel_);
  myStepper2.moveTo(10000000);
  myStepper3.setMaxSpeed(speed_);
  myStepper3.setAcceleration(accel_);
  myStepper3.moveTo(10000000);
  myStepper4.setMaxSpeed(speed_);
  myStepper4.setAcceleration(accel_);
  myStepper4.moveTo(10000000);
  myStepper5.setMaxSpeed(speed_);
  myStepper5.setAcceleration(accel_);
  myStepper5.moveTo(10000000);
  myStepper6.setMaxSpeed(speed_);
  myStepper6.setAcceleration(accel_);
  myStepper6.moveTo(10000000);
  myStepper7.setMaxSpeed(speed_);
  myStepper7.setAcceleration(accel_);
  myStepper7.moveTo(10000000);

  int like_num_;
  uint8_t tick_to_wait = 3;
  Serial.println("runnnn");
  for (;;)
  { 
    digitalWrite(sleep_pin,HIGH);
    if (xQueueReceive(like_num,&like_num_,1) == pdTRUE){
      extract_digit(like_num_);
      
      stepper7_new_number  = (int) arr[0]; //3
      stepper6_new_number  = (int) arr[1]; //3
      stepper5_new_number  = (int) arr[2]; //3
      stepper4_new_number  = (int) arr[3]; //3
      stepper3_new_number  = (int) arr[4]; //3
      stepper2_new_number  = (int) arr[5]; //3
      stepper1_new_number  = (int) arr[6]; //3
    }

    if(stepper1_finish_homing == 0){
      if(xSemaphoreTake(homed_1, tick_to_wait) == pdTRUE){
        stepper1_finish_homing = 1;
        myStepper1.setMaxSpeed(__speed);
        myStepper1.setAcceleration(__accel);
        myStepper1.stop();
        myStepper1.setCurrentPosition(0);
        myStepper1.moveTo(0);
      }
    }
    if(stepper2_finish_homing == 0){
      if(xSemaphoreTake(homed_2, tick_to_wait) == pdTRUE){
        stepper2_finish_homing = 1;
        myStepper2.setMaxSpeed(__speed);
        myStepper2.setAcceleration(__accel);
        myStepper2.stop();
        myStepper2.setCurrentPosition(0);
        myStepper2.moveTo(0);
      }
    }
    if(stepper3_finish_homing == 0){
      if(xSemaphoreTake(homed_3, tick_to_wait) == pdTRUE){
        stepper3_finish_homing = 1;
        myStepper3.setMaxSpeed(__speed);
        myStepper3.setAcceleration(__accel);
        myStepper3.stop();
        myStepper3.setCurrentPosition(0);
        myStepper3.moveTo(0);
      }
    }
    if(stepper4_finish_homing == 0){
      if(xSemaphoreTake(homed_4, tick_to_wait) == pdTRUE){
        stepper4_finish_homing = 1;
        myStepper4.setMaxSpeed(__speed);
        myStepper4.setAcceleration(__accel);
        myStepper4.stop();
        myStepper4.setCurrentPosition(0);
        myStepper4.moveTo(0);
      }
    }
    if(stepper5_finish_homing == 0){
      if(xSemaphoreTake(homed_5, tick_to_wait) == pdTRUE){
        stepper5_finish_homing = 1;
        myStepper5.setMaxSpeed(__speed);
        myStepper5.setAcceleration(__accel);
        myStepper5.stop();
        myStepper5.setCurrentPosition(0);
        myStepper5.moveTo(0);
      }
    }
    if(stepper6_finish_homing == 0){
      if(xSemaphoreTake(homed_6, tick_to_wait) == pdTRUE){
        stepper6_finish_homing = 1;
        myStepper6.setMaxSpeed(__speed);
        myStepper6.setAcceleration(__accel);
        myStepper6.stop();
        myStepper6.setCurrentPosition(0);
        myStepper6.moveTo(0);
      }
    }
    if(stepper7_finish_homing == 0){
      if(xSemaphoreTake(homed_7, tick_to_wait) == pdTRUE){
        stepper7_finish_homing = 1;
        Serial.println("homed!!!!!!!");
        myStepper7.setMaxSpeed(__speed);
        myStepper7.setAcceleration(__accel);
        myStepper7.stop();
        myStepper7.setCurrentPosition(0);
        myStepper7.moveTo(0);
      }
    }
  if(stepper1_finish_homing == 1 && myStepper1.distanceToGo() == 0) {
    // Serial.print("current_state = ");
    // Serial.println(myStepper.currentPosition());
    
    if(stepper1_new_number != stepper1_old_number){
      
        int distant_to_move = stepper1_new_number - stepper1_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        // Serial.println(distant_to_move);
        myStepper1.moveTo(myStepper1.currentPosition() + distant_to_move *(int) tick);
        stepper1_old_number = stepper1_new_number;
    }
  }    
  if(stepper2_finish_homing == 1 && myStepper2.distanceToGo() == 0) {
    // Serial.print("current_state = ");
    // Serial.println(myStepper.currentPosition());
    
    if(stepper2_new_number != stepper2_old_number){
      
        int distant_to_move = stepper2_new_number - stepper2_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        // Serial.println(distant_to_move);
        myStepper2.moveTo(myStepper2.currentPosition() + distant_to_move *(int) tick);
        stepper2_old_number = stepper2_new_number;
    }
  }    
  if(stepper3_finish_homing == 1 && myStepper3.distanceToGo() == 0) {
    // Serial.print("current_state = ");
    // Serial.println(myStepper.currentPosition());
    
    if(stepper3_new_number != stepper3_old_number){
      
        int distant_to_move = stepper3_new_number - stepper3_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        // Serial.println(distant_to_move);
        myStepper3.moveTo(myStepper3.currentPosition() + distant_to_move *(int) tick);
        stepper3_old_number = stepper3_new_number;
    }
  }
  if(stepper4_finish_homing == 1 && myStepper4.distanceToGo() == 0) {
    // Serial.print("current_state = ");
    // Serial.println(myStepper.currentPosition());
    
    if(stepper4_new_number != stepper4_old_number){
      
        int distant_to_move = stepper4_new_number - stepper4_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        // Serial.println(distant_to_move);
        myStepper4.moveTo(myStepper4.currentPosition() + distant_to_move *(int) tick);
        stepper4_old_number = stepper4_new_number;
    }
  }
  if(stepper5_finish_homing == 1 && myStepper5.distanceToGo() == 0) {
    // Serial.print("current_state = ");
    // Serial.println(myStepper.currentPosition());
    
    if(stepper5_new_number != stepper5_old_number){
      
        int distant_to_move = stepper5_new_number - stepper5_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        // Serial.println(distant_to_move);
        myStepper5.moveTo(myStepper5.currentPosition() + distant_to_move *(int) tick);
        stepper5_old_number = stepper5_new_number;
    }
  }
  if(stepper6_finish_homing == 1 && myStepper6.distanceToGo() == 0) {
    // Serial.print("current_state = ");
    // Serial.println(myStepper.currentPosition());
    
    if(stepper6_new_number != stepper6_old_number){
      
        int distant_to_move = stepper6_new_number - stepper6_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        // Serial.println(distant_to_move);
        myStepper6.moveTo(myStepper6.currentPosition() + distant_to_move *(int) tick);
        stepper6_old_number = stepper6_new_number;
    }
  }
  if(stepper7_finish_homing == 1 && myStepper7.distanceToGo() == 0) {
    // Serial.print("current_state = ");
    // Serial.println(myStepper.currentPosition());
    
    if(stepper7_new_number != stepper7_old_number){
      
        int distant_to_move = stepper7_new_number - stepper7_old_number;
        
        while(distant_to_move < 0){
          distant_to_move += 10;
          }
        // Serial.println(distant_to_move);
        myStepper7.moveTo(myStepper7.currentPosition() + distant_to_move *(int) tick);
        stepper7_old_number = stepper7_new_number;
    }
  }
    if(myStepper7.distanceToGo() != 0){
      myStepper7.run();
    }
    if(myStepper6.distanceToGo() != 0){
      myStepper6.run();
    }
    if(myStepper5.distanceToGo() != 0){
      myStepper5.run();
    }
    if(myStepper4.distanceToGo() != 0){
      myStepper4.run();
    }
    if(myStepper3.distanceToGo() != 0){
      myStepper3.run();
    }
    if(myStepper2.distanceToGo() != 0){
      myStepper2.run();
    }
    if(myStepper1.distanceToGo() != 0){
      myStepper1.run();
    }

  }
}

void Task_Data  ( void *pvParameters ){
  // Define Firebase Data object

  for(;;){



    vTaskDelay(pdMS_TO_TICKS(1000));
    // test
    // size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    // Serial.print("Free heap: ");
    // Serial.println(freeHeap);
    // vTaskDelay(pdMS_TO_TICKS(500));
  }
  vTaskDelete(NULL);
}

void running_ap (void *pvParameters){
  WiFi.softAP(ssid, password);
  Serial.print("[+] AP Created with IP Gateway ");
  Serial.println(WiFi.softAPIP());
  pinMode(output26, OUTPUT);
  digitalWrite(output26, LOW);
  server.begin();
  for(;;){
    running_ap();
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}


int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);

  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  
  return addrOffset + 1 + len;
}

int readStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];

  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)

  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}

void connect(){
  readStringFromEEPROM(0, &SSID_Read);
  readStringFromEEPROM(50, &Password_Read);
  readStringFromEEPROM(100, &PageID_Read);
  // // convert to char from string
  String(SSID_Read).toCharArray(WIFI_SSID,SSID_Read.length() + 1);
  String(Password_Read).toCharArray(WIFI_PASSWORD,Password_Read.length() + 1);
  String(PageID_Read).toCharArray(PAGE_ID,PageID_Read.length() + 1);
  //
  Serial.println("SSID == " + SSID_Read);
  Serial.println("Password == " + Password_Read);
  Serial.println("PageID == " + PageID_Read);
  // connect wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      vTaskDelay(pdMS_TO_TICKS(300));
      if (millis() - ms >= 10000){
        break;
        Wifi_status = false;
        Serial.println("cant connect wifi");
        // To_connect_wifi = 0;
      }
      else {
        // To_connect_wifi = 1;
      }
  }
  if (Wifi_status == true && To_connect_wifi == 1){
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
}


void running_ap(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            
            if (header.indexOf("GET /zero/on") >= 0) {
              Serial.println("GPIO 26 on");
              LED_state = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /zero/off") >= 0) {
              Serial.println("GPIO 26 off");
              LED_state = "off";
              digitalWrite(output26, LOW);
            }
            else {
              Serial.println("-----------------------");
              Serial.println(header.c_str());
              String SSID = header.substring(header.indexOf("SSID=") + 5 ,header.indexOf("&Password"));
              String Password = header.substring(header.indexOf("Password=") + 9,header.indexOf("&PageID"));
              String PageID = header.substring(header.indexOf("PageID=") + 7,header.indexOf(" HTTP"));
              
              SSID.replace("+"," ");

              Serial.print("SSID = ");
              Serial.println(SSID);
              Serial.print("Password = ");
              Serial.println(Password);
              Serial.print("PageID = ");
              Serial.println(PageID);
              
              int Offset = writeStringToEEPROM(0, SSID);  // 50 bytes
              Serial.println("SSID = " + SSID + "\n length = " + Offset);
              Offset = writeStringToEEPROM(50, Password); // 50 bytes
              Serial.println("Password = " + Password + "\n length = " + int(Offset - 50));
              Offset = writeStringToEEPROM(100, PageID); // 50 bytes
              Serial.println("PageID = " + PageID + "\n length = " + int(Offset - 50));
              EEPROM.commit();
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");  
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Testing ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO zero - State " + LED_state + "</p>");
            // If the LED_state is off, it displays the ON button       
            if (LED_state=="off") {
              client.println("<p><a href=\"/zero/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/zero/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            client.println("<form action=\"/page_like_counter\">");

            client.println("<label for=\"SSID\">SSID:</label>");
            client.println("<input type=\text\" id=\"SSID\" name=\"SSID\" placeholder=\"wifi name\"><br><br>");

            client.println("<label for=\"Password\">Password:</label>");
            client.println("<input type=\text\" id=\"Password\" name=\"Password\" placeholder=\"wifi password\"><br><br>");

            client.println("<label for=\"PageID\">PageID:</label>");
            client.println("<input type=\text\" id=\"PageID\" name=\"PageID\" placeholder=\"pageid\"><br><br>");

            client.println("<input type=\"submit\" value=\"Submit\">");
            client.println("</form>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}



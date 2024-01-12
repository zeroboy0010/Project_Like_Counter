#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
// 7 segment
#include <max7219.h>
#define LEFT 0
#define RIGHT 1
MAX7219 max7219;

// eeprom
#include <EEPROM.h>
String Input;
String Output;
String Password;
String SSID;
String PageID;
String Password_Read;
String SSID_Read;
String PageID_Read;
int writeStringToEEPROM(int addrOffset, const String &strToWrite);
int readStringFromEEPROM(int addrOffset, String *strToRead);
void store_data();
//
// Provide the token generation process info.
#include <addons/TokenHelper.h>

/* 1. Define the WiFi credentials */
char WIFI_SSID[20];
char WIFI_PASSWORD[20];

bool Wifi_status = true;
bool taskCompleted = false;

/* 2. Define the API Key */
#define API_KEY "AIzaSyBCsGv1SVYiKzgnKmEZSgx9ZbEc_A6gN6w"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "like-counter-3d752"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "zeroeverything001@gmail.com"
#define USER_PASSWORD "11112222"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long dataMillis = 0;
char Like_str[8];

// connect wifi and firestore
void connect();

void setup()
{
  EEPROM.begin(150);
  Serial.begin(9600);
  // condition here
  Serial.println("type \"c=\" if you want to config!! \n you have 5 seconds!!");
  while (millis() < 5000){
    store_data();
  }
  if (Wifi_status == true ){
    connect();
  }
  max7219.Begin();
}

void loop()
{

    // Firebase.ready() should be called repeatedly to handle authentication tasks.
    if(Serial.available() == 0 && Wifi_status == false){
      store_data();
    }
    if (Firebase.ready() && (millis() - dataMillis > 1000 || dataMillis == 0) && Wifi_status == true)
    {
        dataMillis = millis();
        String documentPath = "project_like_counter/device_001";
        if (!taskCompleted)
        {
            taskCompleted = true;

            // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create.ino
            FirebaseJson content;

            content.set("fields/like/integerValue", "0");
            content.set("fields/pageID/stringValue",PageID_Read);
            content.set("fields/state/stringValue","running");

            // info is the collection id, countries is the document id in collection info.
            

            Serial.print("Create document... ");

            if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw()))
                Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
            else
                Serial.println(fbdo.errorReason());

            if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(),"pageID"))
                Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
            else
                Serial.println(fbdo.errorReason());
        }
        String mask = "";

        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

        Serial.print("Get a document... ");

        if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str())){
            
            // Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());

            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject(fbdo.payload().c_str());
            int Like = root["fields"]["like"]["integerValue"];
            String Name = root["fields"]["pageID"]["stringValue"];
            (String(Like)).toCharArray(Like_str,(String(Like)).length()+1);
            Serial.print("Like = ");
            Serial.println(Like);
            Serial.print("Name = ");
            Serial.println(Name);
            Serial.println("----------------------------------------");
            max7219.Clear();
            max7219.DisplayText(Like_str, RIGHT);
        }
        else
            
            Serial.println(fbdo.errorReason());
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
      delay(300);
  }
  if (Wifi_status == true){
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    // Limit the size of response payload to be collected in FirebaseData
    fbdo.setResponseSize(2048);

    Firebase.begin(&config, &auth);

    Firebase.reconnectWiFi(true);
  }
}

void store_data(){
  Input = Serial.readStringUntil('\n');
  for(int i = 0;i<Input.length();i++){
      if (Input[i] == '=' || Input[i] == ' ') {
          Output.toLowerCase();
          break;
      }
      else{
          Output+=Input[i];
      }
  }
  
  if (Output == "ssid") {
      int a = Input.indexOf("\"");
      Input.remove(0,a+1);
      a = Input.indexOf("\"");
      Input.remove(a);
      SSID = Input;
      int Offset = writeStringToEEPROM(0, SSID);  // 50 bytes
      Serial.println("SSID = " + SSID + "\n length = " + Offset);
  }
  else if(Output == "password"){
      int a = Input.indexOf("\"");
      Input.remove(0,a+1);
      a = Input.indexOf("\"");
      Input.remove(a);
      Password = Input;
      int Offset = writeStringToEEPROM(50, Password); // 50 bytes
      Serial.println("Password = " + Password + "\n length = " + int(Offset - 50));
  }
  else if(Output == "pageid"){
      int a = Input.indexOf("\"");
      Input.remove(0,a+1);
      a = Input.indexOf("\"");
      Input.remove(a);
      PageID = Input;
      int Offset = writeStringToEEPROM(100, PageID);  // 50 bytes
      Serial.println("PageID = " + PageID + "\n length = " + int(Offset - 100));
  }
  else if(Output == "save"){
    Serial.println("save!!");
    EEPROM.commit();
    delay(1000);
    Wifi_status = true;
    connect();
  }
  else if(Output == "c"){
    Serial.println("Ok got that !!!");
    Wifi_status = false;
  }
  else{
    Input.clear();
  }
  
  Output.clear();
}
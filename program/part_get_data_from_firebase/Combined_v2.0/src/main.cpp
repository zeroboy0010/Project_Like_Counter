#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
// 7 segment
#include <max7219.h>
#define LEFT 0
#define RIGHT 1
#define Input_Button 15
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

// Replace with your network credentials
const char* ssid     = "Like_counter_demo";
const char* password = "12345678";
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;


// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = LED_BUILTIN;

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long dataMillis = 0;
char Like_str[8];

int connected_wifi;

// connect wifi and firestore
void connect();
void running_ap();


void setup()
{
  EEPROM.begin(150);
  Serial.begin(9600);
  pinMode(Input_Button,INPUT);

  for (int i=0; i<50;i++){
    Serial.print(".");
    if (digitalRead(Input_Button) == 1){
      connected_wifi = 0;
      break;
    }
    else {
      connected_wifi = 1;
    }
    delay(100);
  }

  Serial.println("---------------------");
  // condition here

  // WiFi.mode(WIFI_AP_STA);
  if (connected_wifi == 0){
    WiFi.softAP(ssid, password);
    Serial.print("[+] AP Created with IP Gateway ");
    Serial.println(WiFi.softAPIP());
    pinMode(output26, OUTPUT);
    digitalWrite(output26, LOW);
    server.begin();
  }
  if (Wifi_status == true && connected_wifi == 1){
    connect();
  }


  max7219.Begin();
}

void loop()
{
  if (connected_wifi == 0){
    running_ap();
  }
  
    // Firebase.ready() should be called repeatedly to handle authentication tasks.
    if (Firebase.ready() && (millis() - dataMillis > 1000 || dataMillis == 0) && Wifi_status == true && connected_wifi == 1 )
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
      if (millis() - ms >= 10000){
        break;
        Serial.println("cant connect wifi");
        connected_wifi = 0;
      }
      else {
        connected_wifi = 1;
      }
  }
  if (Wifi_status == true && connected_wifi == 1){
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
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /zero/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            }
            else {
              Serial.println("-----------------------");
              Serial.println(header.c_str());
              String SSID = header.substring(header.indexOf("SSID=") + 5 ,header.indexOf("&Password"));
              String Password = header.substring(header.indexOf("Password=") + 9,header.indexOf("&PageID"));
              String PageID = header.substring(header.indexOf("PageID=") + 7,header.indexOf(" HTTP"));
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
            client.println("<p>GPIO zero - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
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
#include <Arduino.h>
#include <EEPROM.h>

String Input;
String Output;
String Password;
String SSID;
String PageID;

int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);

  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  EEPROM.commit();
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
void store_data();

void setup() {
    Serial.begin(9600);
    EEPROM.begin(150);
}
void loop() {
    if(Serial.available() == 0){
      store_data();
    }
}

void store_data(){
  Input = Serial.readStringUntil('\n');
  for(int i = 0;i<Input.length();i++){
      if (Input[i] == '=' || Input[i] == ' ') {
          break;
      }
      else{
          Output+=Input[i];
      }
  }
  if (Output == "SSID") {
      int a = Input.indexOf("\"");
      Input.remove(0,a+1);
      a = Input.indexOf("\"");
      Input.remove(a);
      SSID = Input;
      int Offset = writeStringToEEPROM(0, SSID);  // 50 bytes
      Serial.println("SSID = " + SSID + "\n length = " + Offset);
  }
  else if(Output == "Password"){
      int a = Input.indexOf("\"");
      Input.remove(0,a+1);
      a = Input.indexOf("\"");
      Input.remove(a);
      Password = Input;
      int Offset = writeStringToEEPROM(50, Password); // 50 bytes
      Serial.println("Password = " + Password + "\n length = " + int(Offset - 50));
  }
  else if(Output == "PageID"){
      int a = Input.indexOf("\"");
      Input.remove(0,a+1);
      a = Input.indexOf("\"");
      Input.remove(a);
      PageID = Input;
      int Offset = writeStringToEEPROM(100, PageID);  // 50 bytes
      Serial.println("PageID = " + PageID + "\n length = " + int(Offset - 100));
  }
  Output.clear();
}
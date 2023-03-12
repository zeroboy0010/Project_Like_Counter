#include <Arduino.h>


String Input;
String Output;
String Password;
String SSID;
String PageID;
void setup() {
    Serial.begin(9600);
}
void loop() {
    if(Serial.available() == 0){
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
            Serial.println("SSID = " + SSID);
        }
        else if(Output == "Password"){
            int a = Input.indexOf("\"");
            Input.remove(0,a+1);
            a = Input.indexOf("\"");
            Input.remove(a);
            Password = Input;
            Serial.println("Password = " + Password);
        }
        else if(Output == "PageID"){
            int a = Input.indexOf("\"");
            Input.remove(0,a+1);
            a = Input.indexOf("\"");
            Input.remove(a);
            PageID = Input;
            Serial.println("PageID = " + Password);
        }
        Output.clear();
    }
}
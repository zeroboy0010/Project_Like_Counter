#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
// 7 segment
#include <max7219.h>
#define LEFT 0
#define RIGHT 1
MAX7219 max7219;

// Provide the token generation process info.
#include <addons/TokenHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Bunmoly-1"
#define WIFI_PASSWORD "55558888"

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
void setup()
{
    
    Serial.begin(9600);
    max7219.Begin();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
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

void loop()
{

    // Firebase.ready() should be called repeatedly to handle authentication tasks.

    if (Firebase.ready() && (millis() - dataMillis > 1000 || dataMillis == 0))
    {
        dataMillis = millis();

        String documentPath = "project_like_counter/page_001";
        String mask = "";

        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

        Serial.print("Get a document... ");

        if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str())){
            
            // Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());

            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject(fbdo.payload().c_str());
            int Like = root["fields"]["like"]["integerValue"];
            String Name = root["fields"]["name"]["stringValue"];
            (String(Like)).toCharArray(Like_str,Name.length()+1);
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
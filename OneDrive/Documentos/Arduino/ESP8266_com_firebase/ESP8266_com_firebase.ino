#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "xxxxxxxx"
#define WIFI_PASSWORD "xxxxxxxx"

//API Key do projeto: ANONIMO
#define API_KEY "AIzaSyCZ51ugW42XdR-jJ6OAJhYTPLcjsABbalw"

// url do banco de dados
#define DATABASE_URL "https://deia-device-964f8-default-rtdb.firebaseio.com/" 

//cria uma pasta no banco de dados
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
float leitura;
float floatValue;
bool signupOK = false;

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    
    leitura = (analogRead(A0)*3.3/1023);
    
    // envia um dado do tipo int para o Firebase na pasta test/int; ".setInt"
    if (Firebase.RTDB.setInt(&fbdo, "test/int", count)){
      Serial.println("PASSED_INT");

    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    
    //envia um dado do tipo float para o Firebase na pasta test/float; ".setFloat"
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", leitura)){
      Serial.println("PASSED_FLOAT");
     
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    //recupera dados do firebase; ".getFloat"
    if (Firebase.RTDB.getFloat(&fbdo, "/test/float")) {
      if (fbdo.dataType() == "float") {
        floatValue = fbdo.floatData();
        Serial.println(floatValue);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
  }
}

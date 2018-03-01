#include <UniversalTelegramBot.h>
#include <ESP8266WiFi.h>
#include <IFTTTMaker.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Servo.h> //include servo library

//---WiFi Settings---
char ssid[] = "";
char password[] = "";

//---IFTTT Webhooks Config---
#define KEY ""
#define EVENT_NAME "gate_opened"

//---Telegram Config---
#define BOT_Token ""
#define CHAT_ID ""

//--- SSL Client needed for both libraries
WiFiClientSecure client;

IFTTTMaker ifttt(KEY, client);
UniversalTelegramBot bot(BOT_token, client);

String ipAddress ="";

volatile bool telegramButtonPressedFlag = false;
volatile bool iftttButtonPressedFlag = false;

Servo myservo; //define servo as servo
const int waterSens = A0;//set water sensor to A0
int pos = 0;//define servo position


  
  
void setup() {
  
  Serial.begin(9600);
  myservo.attach(9);//attach servo to pin 9
  
  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  ipAddress = ip.toString();
}


void triggerIftttEvent() {
  if(ifttt.triggerEvent(EVENT_NAME, ssid, ipAddress)){
    Serial.println("IFTTT Successfully sent");
  }
  iftttButtonPressedFlag = false;
}

void sendTelegramMessage() {
  String message = "SSID:  ";
  message.concat(ssid);
  message.concat("\n");
  message.concat("IP: ");
  message.concat(ipAddress);
  message.concat("\n");
  if(bot.sendMessage(CHAT_ID, message, "Markdown")){
    Serial.println("TELEGRAM Successfully sent");
  }
  telegramButtonPressedFlag = false;
}


void loop() {
  int sensorValue = analogRead(waterSens);//read the water sensor value
          
  sensorValue = map(sensorValue, 0, 1023, 0, 180); 
     if (sensorValue >= 80) {
        for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
      
     }
     
  else{
    myservo.write(0);
  }
                    
  
  Serial.println(sensorValue);
  delay(20);      }  
{
  if ( iftttButtonPressedFlag ) {
    triggerIftttEvent();
  }

  if ( telegramButtonPressedFlag ) {
    sendTelegramMessage();
  }
}

// Import required libraries
#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ESPServer.h>
#include "time.h"
#include <Servo.h>
#include <AlarmBuzzer.h>

int DISPENSE_INTERVAL = 1500;
int PRESSURE_SENSOR = A1;
int SERVO_PIN = 18;
int pressure = 0;
int LED_PIN = 2;
int WIFI_LED_PIN = 13;
int IR_EMITTER_PIN = 26;
int IR_RECEIVER_PIN = A0;
// State control 
bool remind = false;
bool dispense = false;
bool presence = false;

// for obtaining local time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 36000;
const int   daylightOffset_sec = 3600;
int cur_min = 0;
int cur_hr = 0;
// Replace with your network credentials
const char* ssid = "nick";
const char* password = "12345678";
struct tm timeinfo;

// Servo
Servo servo;
void fetchLocalTime() {
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
}

void checkCurTime() {
  fetchLocalTime();
  cur_hr = timeinfo.tm_hour;
  cur_min = timeinfo.tm_min;
  if(cur_min == alarm_min && cur_hr == alarm_hr) {
    remind = true;
    dispense = true;
  }
}

void dropServoArm() {
  servo.attach(SERVO_PIN);
  dispense = false;
  delay(1000);
  Serial.println("Dropping pill box..");
  servo.write(90);
  Serial.println("Reset servo arm");
  delay(1000);
  servo.write(0);
  delay(1000);
  servo.detach();
}

void startAlarm() {
  // dispense pill box
  if(dispense) {
    dropServoArm();
  }
  // ring the buzzer

  if(remind) {
    // LED turns red
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
    digitalWrite(LED_PIN, HIGH);
    ringBuzzer(alarm_notes, sizeof(alarm_notes)/sizeof(alarm_notes[0]));
    delay(200);
    servo.write(0);
  }

  // check pressure
  int pressure = analogRead(PRESSURE_SENSOR);
  if (remind) {
    if(pressure > 0) {
      presence = true;
    }

    if(presence && pressure == 0) {
      remind = false;
      presence = false;
      digitalWrite(LED_PIN, LOW);
      alarm_min = 0;
      alarm_hr = 0;
    }
    Serial.println(pressure);
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);  // LED
  pinMode(WIFI_LED_PIN, OUTPUT);
  pinMode(IR_EMITTER_PIN, OUTPUT);
  servo.attach(SERVO_PIN);   // mini servo
  // config time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  fetchLocalTime();
  // Connect to Wi-Fi

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("..");
  }
  Serial.println(WiFi.localIP()); // Print ESP Local IP Address

  createRoutes();
  server.begin();
}
  
void loop() {
  digitalWrite(IR_EMITTER_PIN, HIGH);
  float res = analogRead(IR_RECEIVER_PIN);
  Serial.println(res);
  delay(200);
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(WIFI_LED_PIN, HIGH);
  } else {
    digitalWrite(WIFI_LED_PIN, LOW);
  }
  if(!remind) {
    servo.write(0);
    checkCurTime();
  }
  else {
    startAlarm();
  }
}
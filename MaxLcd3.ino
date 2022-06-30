//Define Blynk
#define BLYNK_TEMPLATE_ID           "TMPLkMHYWpOj"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "dIBlz6ohfBzk0iOpzsbPQb3Sph33-iJU"
#define BLYNK_PRINT Serial

//Include Library
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define BLYNK_PRINT Serial
#define REPORTING_PERIOD_MS     500

//Declare I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

//SSID and Password information
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "realme";
char pass[] = "12345678";
PulseOximeter pox;

const int numReadings=15;
float filterweight=0.5;
uint32_t tsLastReport = 0;
uint32_t last_beat=0;
int readIndex=0;
int average_beat=0;
int average_SpO2=0;
bool calculation_complete=false;
bool calculating=false;
bool initialized=false;
byte beat=0;
ESP8266WebServer server(80);
BlynkTimer timer;

void myTimerEvent()
{
  //Display data on Blynk
  Blynk.virtualWrite(V4, average_SpO2);
  Blynk.virtualWrite(V5, average_beat);

  //Notify on Blynk if data meet certain condition
  if ( average_beat > 100) 
  {
    Blynk.logEvent("bpm_alert","BPM reading is high!!!");
  }
  else
  {Blynk.logEvent("bpmok","BPM reading normal");
  }
  if (average_SpO2 < 100) 
  {
    Blynk.logEvent("spo2_alert","Oxygen level reading is low!!!");
  }
}

void onBeatDetected()
{
  //Detect pulse from oximeter
  show_beat();
  last_beat=millis();
}

void show_beat() 
{
  //Print pulse status
  lcd.setCursor(0, 0);
  if (beat==0) {
    lcd.print("_");
    beat=1;
  } 
  else
  {
    lcd.print("^");
    beat=0;
  }
}

void initial_display() 
{
  //Display command on LCD
  if (not initialized) 
  {
    show_beat();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Place Finger");  
    lcd.setCursor(0,1);
    lcd.print("On The Sensor..");
    initialized=true;
  }
}

void display_calculating(int j)
{
  //Measure SPO2 and BPM
  if (not calculating) {
    calculating=true;
    initialized=false;
  }
  show_beat();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Measuring...."); 
  lcd.setCursor(0,1);
  for (int i=0;i<=j;i++) {
  lcd.print(".");
  }
}
void display_values()
{
  //Display data on LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BPM  =");
  lcd.print(average_beat);
  lcd.print(" BPM");
  lcd.setCursor(0,1);
  lcd.print("SpO2 =");
  lcd.print(average_SpO2);
  lcd.print(" %"); 
}

void calculate_average(int beat, int SpO2) 
{
  //Calculate SPO2 and BPM
  if (readIndex==numReadings) {
    calculation_complete=true;
    calculating=false;
    initialized=false;
    readIndex=0;
    display_values();
  }
  
  if (not calculation_complete and beat>30 and beat<220 and SpO2>50 and SpO2<100) {
    average_beat = filterweight * (beat) + (1 - filterweight ) * average_beat;
    average_SpO2 = filterweight * (SpO2) + (1 - filterweight ) * average_SpO2;
    readIndex++;
    display_calculating(readIndex);
  }
}

void setup()
{
  //Begin serial print
  Serial.begin(115200);
  //Begin Blynk
  Blynk.begin(auth, ssid, pass);
  //Start LCD
  lcd.init(); 
  lcd.backlight();
  timer.setInterval(1000L, myTimerEvent);
  lcd.setCursor(0,0);
  lcd.println("Connecting to");
  lcd.setCursor(0,1);
  lcd.println(ssid);
  Serial.println("Connecting to");
  Serial.println(ssid);

  //Connecting to WiFi
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("--> WiFi Connected <--");
  lcd.clear();
  lcd.println(" WiFi Connected ");

  //Connecting to Oximeter
  Serial.print("Initializing pulse oximeter..");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Initializing pulse oximeter..");
  if (!pox.begin()) {
    lcd.setCursor(0,1);
    lcd.println("FAILED");
    Serial.println("FAILED");
    for (;;);
  } else {
    lcd.setCursor(0,1);
    lcd.println("SUCCESS");
    Serial.println("SUCCESS");
  }
  //Oximeter begin
  pox.begin();
  pox.setOnBeatDetectedCallback(onBeatDetected);
  initial_display();
}

void loop()
{
  //Obtain SPO2 and BPM from sensor
    pox.update();
    if ((millis() - tsLastReport > REPORTING_PERIOD_MS) and (not calculation_complete)) {
        calculate_average(pox.getHeartRate(),pox.getSpO2());
        tsLastReport = millis();
    }
    if ((millis()-last_beat>5000)) {
      calculation_complete=false;
      average_beat=0;
      average_SpO2=0;
      initial_display();
    }
    //Start Blynk
      Blynk.run();
      timer.run();
}

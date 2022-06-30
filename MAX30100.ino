#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000

float BPM, SpO2;

PulseOximeter pox;

void setup() 
{
  Serial.begin(115200);
  delay(100);
      Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) 
  {
    Serial.println("FAILED");
    for (;;);
  }
  else 
  {
    Serial.println("SUCCESS");
  }
}

void loop() 
{
  pox.update();
  {
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();

    Serial.print("BPM: ");
    Serial.println(BPM);

    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");

    Serial.println("*********************************");
    Serial.println();
      delay(100);
  }
}

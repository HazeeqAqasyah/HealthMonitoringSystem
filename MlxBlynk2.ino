//Declare blynk
#define BLYNK_TEMPLATE_ID           "TMPLkMHYWpOj"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "dIBlz6ohfBzk0iOpzsbPQb3Sph33-iJU"

//Include Library
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#define BLYNK_PRINT Serial
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WebServer.h>
#define REPORTING_PERIOD_MS     1000 
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

double temp_amb;
double temp_obj;

//Define SSID and Password
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "realme";
char pass[] = "12345678";
ESP8266WebServer server(80);
BlynkTimer timer;

void setup()
{
  //Begin Serial Print
  Serial.begin(115200);
  //Begin Temperature Sensor
  mlx.begin();
  //Connect to Blynk
  Blynk.begin(auth, ssid, pass);
  Serial.println("Temperature Sensor MLX90614");
}
 
void loop()
{
  //mlx.readAmbientTempF() , mlx.readObjectTempF() )
  //Run Blynk
  Blynk.run();
  //Obtain Room temperature and object temperature data from sensor
  temp_amb = mlx.readAmbientTempC();
  temp_obj = mlx.readObjectTempC();

  //Display data to serial print
  Serial.print("Room Temp = ");
  Serial.println(temp_amb);
  Serial.print("Object temp = ");
  Serial.println(temp_obj);

  //Display data to Blynk
  Blynk.virtualWrite(V6, temp_amb);
  Blynk.virtualWrite(V7, (temp_obj));
  
  delay(1000);
}

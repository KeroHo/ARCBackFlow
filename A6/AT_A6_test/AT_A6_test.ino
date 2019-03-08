#include "Arduino.h"
#include "ESP8266.h"

ESP8266 cell;
// define Sensor Pin
int sensorPin0 = A0; //sensor 1

// define var for send dat to website
String host = "constbackflow.000webhostapp.com"; // constbackflow.000webhostapp.com  BackFlow Test website
const int httpPort   = 80;
String uri = "/espost2.php";
//String uriSens = "/sensor.html";
String data;

char returnVal ;
String userInput;

void setup() {
Serial.begin(38400);
Serial1.begin(38400);
delay(300); 
Serial1.println("AT");

}

void loop()
{


////////  
if(Serial.available() > 0)
{
  userInput = Serial.readString(); 
  
  Serial1.println(userInput);
  delay(200);

}

if(Serial1.available() > 0)
{
returnVal = Serial1.read();   
delay(500);
Serial.print(returnVal);

}

}

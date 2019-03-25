/*
  Modified from (Simple POST created 14 Feb 2016 by Tom Igoe)
  Edited by Hieu Ho
  American River College - Beaver Constant Backflow Monitor
  updated March 25
*/
///DEVICE ID  in Arduino Secrets
#include "arduino_secrets.h"
#define TINY_GSM_MODEM_A6
// Increase RX buffer if needed
//#define TINY_GSM_RX_BUFFER 512

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

//RTC_DS3231 
#include <RTClib.h>
#include <Wire.h>
RTC_DS3231 rtc;
//

// Uncomment this if you want to see all AT commands
#define DUMP_AT_COMMANDS

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
#define TINY_GSM_DEBUG SerialMon

// or Software Serial on Uno, Nano
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3); // RX, TX

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "hologram";
const char user[] = "";
const char pass[] = "";

// Server details
const char server[] = "constbackflow.000webhostapp.com";
const char resource[] = "/sensor.html";
//const char uri[] = "/espost2.php";
const int  port = 80;
String uri = "/espost2.php";

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
int timeoutCounter = 0;
//HttpClient http(client, server, port);


String response;
int statusCode = 0;

// define Sensor Pin
int sensorPin0 = A0; //sensor 1

void setup() 
{
  // RTC beigin
  rtc.begin();
  
  // Set console baud rate
  SerialMon.begin(38400);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(38400);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print(F("Modem: "));
  SerialMon.println(modemInfo);
  

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  pinMode(sensorPin0, INPUT);
}



void loop() {
  // Waiting of Nework
  SerialMon.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  // Connect to Hologram APN
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  SerialMon.println(" ");
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }


  //A loop to Update data to website

  //for (int i = 0; i < 5; i++)
  {
    

    SerialMon.println("Start Loop");

    // read data from PressureSensor
    float sensorVoltage0 = analogRead(sensorPin0);
    int psi0 = ((sensorVoltage0 - 95) / 204) * 50;

    String temp = String(psi0);
    String psi1 = String(psi0 + 56);
    String psi2 = String(psi0 + 12);
    //data ="p1=" + temp + "&p2=" + psi1 + "&p3=" + psi2;

//  Testing Real Time Clock on A6
////////////////////////////////////////////
//    modem.sendAT("+CCLK?");
//    String res;
//    if (modem.waitResponse(1000L, res) != 1) {
//      return "";
//    }
//    //res.replace(GSM_NL "OK" GSM_NL, "");
//    //res.replace(GSM_NL, " ");
//    res.trim();
//    SerialMon.println (String("DateTime: ") + res);

//////////////////////////////////////

    SerialMon.println("making POST request");
    String contentType = "application/x-www-form-urlencoded";
    String postData ="date=" + getDate()+ "&time=" + getTime()+ "&ID=" + DEVICE_ID + "&p1=" + temp + "&p2=" + psi1 + "&p3=" + psi2;
    //String postData ="date=" + getDate()+ "&p1=" + temp + "&p2=" + psi1 + "&p3=" + psi2;
    
    //delay(5000);
    // Check data with Serial.
    SerialMon.println(postData);

    ////////////////////////////////////////////////
    //Post Format from Firmware Script

    
    //String postRequest = "POST " + uri + " HTTP/1.0\r\n" 
    //+ "Host: " + server + "\r\n" 
    //+ "User-Agent: Arduino/1.8.4\r\n" 
    //+ "Accept: *" + "/" + "*\r\n" 
    //+ "Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n" 
    //+ "Content-Length: " + String(postData.length()) 
    //+ "\r\n" + "\r\n" + postData;
      
    ////////////////////////////////////////////////
    
    //SerialMon.print(postRequest);
    //uint8_t buffer[postRequest.length()];
    //postRequest.toCharArray(buffer, postRequest.length() + 1);
    //String postString = "+CIPSEND=0,"+ String(postRequest.length()) + "," + postRequest;
    //uint8_t buffer[postString.length()];
    //postString.toCharArray(buffer, postString.length() + 1);
    ///////////////////////////////////////////////
    //SerialMon.println(postString);
    
    if(modem.isGprsConnected())//Check GPRS connection status
    {
        SerialMon.println("Start Sending");  
        client.connect(server, port);
//        {
//          SerialMon.println("TCP Fail");
//            return;
//        }
            
        client.print(String(("POST " + String(uri) + " HTTP/1.1\r\n")));
        client.print(String(("Host: " + String(server) + "\r\n")));
        client.print(String(("User-Agent: Arduino/1.8.4\r\n")));
        //client.print(String(("Accept: */*\r\n") ));
        client.print(String(("Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n" ) ));
        client.print(String(("Content-Length: " + String( postData.length() ) )));
        client.print(String(("\r\n\r\n" + postData)));
        SerialMon.println("End Sending");
        client.stop();
        SerialMon.println(F("Server disconnected"));
       
    }

    //Modem Reset Function
    ++timeoutCounter;
    if (timeoutCounter >= TIMEOUT_LIMIT)
    {
      timeoutCounter = 0;
      // Restart takes quite some time
      // To skip it, call init() instead of restart()
      SerialMon.println(F("Initializing modem..."));
      modem.restart();
    
      String modemInfo = modem.getModemInfo();
      SerialMon.print(F("Modem: "));
      SerialMon.println(modemInfo);
    }

    //Checking POST Response////////////////
    //SerialMon.print("Status code: ");
    //SerialMon.println(statusCode);
    //SerialMon.print("Response: ");
    //SerialMon.println(response);
    
    modem.gprsDisconnect();
    modem.sendAT("+CIPSHUT=0");
    if (!modem.isGprsConnected()) 
    {
      SerialMon.println("GPRS disconnected");
    } 
    else 
    {
      SerialMon.println("GPRS disconnect: Failed.");
    }

    SerialMon.print("Wait");
    SerialMon.print(READ_INTERVAL);
    SerialMon.print("Minutes");
    delay(READ_INTERVAL*60*1000);

    }//End of Internal For loop
    

    //Serial.println("Wait ANOTHER five seconds");
    //delay(5000);
  
}//End of void loop()


String getTime()
{
  DateTime now = rtc.now();

//  Serial.print(now.hour(), DEC);
//  Serial.print(':');
//  Serial.print(now.minute(), DEC);
//  Serial.print(':');
//  Serial.println(now.second(), DEC);
  //String timeStamp = String(now.year())+ "%2F" + now.month() + "%2F" + now.day() + "%20" + now.hour() + ':' + now.minute() + ':' + now.second();
  return String(now.hour()) + ':' + now.minute() + ':' + now.second();
}

String getDate()
{
  DateTime now = rtc.now();

//  Serial.print(now.year(), DEC);
//  Serial.print('/');
//  Serial.print(now.month(), DEC);
//  Serial.print('/');
//  Serial.print(now.day(), DEC);
  //String timeStamp = String(now.year())+ "%2F" + now.month() + "%2F" + now.day() + "%20" + now.hour() + ':' + now.minute() + ':' + now.second();
  return String(now.year())+ "-" + now.month() + "-" + now.day();
}

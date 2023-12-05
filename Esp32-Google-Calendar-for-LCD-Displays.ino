/* 
This is a fork of the E-Ink Family Calendar project by Argion /  Kristian Thorsted Madsen.
https://www.instructables.com/id/E-Ink-Family-Calendar-Using-ESP32/

The project was written for a diymore ESP32 WROOM 32 Nodemcu Development Board microcontroller and a 20x4 LCD. 
Also note, that the project is dependent on you uploading a google script, to fetch and sort the calendar entries.

The basic configuration happens in the config.h file. Enter your Google Scripts API and your WiFi credentials
*/

#include <string>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include "time.h"

#include "config.h"
//#include <SPIFFS.h>
//#include <FS.h>

#define PIN       33 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 2 // Popular NeoPixel ring size


//Create Objects for LCD and HTTPClient
LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 20, 4);
HTTPClient http;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Function for retrieving Calendar Entries
bool displayCalendar() {

  String hours;
  String minutes;
  String time; 

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  hours = timeinfo.tm_hour;
  minutes = timeinfo.tm_min;
  time = hours + ":" + minutes;

  //Getting calendar from your published google script
  Serial.println("Getting calendar");
  Serial.println(calendarRequest);

  //Connect to Google Script and call the API
  http.end();
  http.setTimeout(20000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if (!http.begin(calendarRequest)) {
    Serial.println("Cannot connect to google script" + calendarRequest);
    return false;
  }
  Serial.println("Connected to google script");
  int returnCode = http.GET();
  Serial.print("Returncode: ");
  Serial.println(returnCode);
  String response = http.getString();
  Serial.println(response);

  //Display Events
  int sa[4], r = 0, t = 0;
  float p = 0;
  String events = response;
  String show;
    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.setBrightness(30);
    pixels.show();   // Send the updated pixel colors to the hardware.
  }
  if (events.length() == 0)  //If no more events display message
  {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("++++++++++++++++++++");
    LCD.setCursor(0, 1);
    LCD.print("+Keine Termine mehr+");
    LCD.setCursor(0, 2);
    LCD.print("+  Geh nach Hause  +");
    LCD.setCursor(0, 3);
    LCD.print("++++++++++++++++++++");
  } else {
    LCD.clear();
    for (int i = 0; i < events.length(); i++)  //Loop through events with defined maximum and display
    {
      if (events.charAt(i) == ';') {
        show = events.substring(r, i);

        if (t == 0 || t == 2){
        if(time >= show.substring(0, 5))
          {
              pixels.setPixelColor(p, pixels.Color(150, 0, 0));
              pixels.setBrightness(30);
              pixels.show();   // Send the updated pixel colors to the hardware.         
          }
        }
        LCD.setCursor(0, t);
        LCD.print(show.substring(0, 19));
        r = (i + 1);
        p = (p+0.5);
        t++;
      }
    }
  }
  return true;
}

//Function for WiFi Connection
bool connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  return true;
}

void setup() {

  //Initialize board
  Serial.begin(115200);
  
  //Initilaize LCD
  LCD.init();
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  connectWiFi();

  //Setup WiFi Connection
  //LCD.backlight();
  //LCD.setCursor(0, 0);
  //LCD.print("++++++++++++++++++++");
  //LCD.setCursor(0, 1);
  //LCD.print("+Connecting to WiFi+");
  //LCD.setCursor(0, 2);
  //LCD.print("+    Please wait   +");
  //LCD.setCursor(0, 3);
  //LCD.print("++++++++++++++++++++");
  //delay(1000);

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

//Main Code
void loop() {

  pixels.clear(); // Set all pixel colors to 'off'

  //Update message
  LCD.clear();
  LCD.backlight();
  LCD.setCursor(0, 0);
  LCD.print("++++++++++++++++++++");
  LCD.setCursor(0, 1);
  LCD.print("+Aktualisiere Daten+");
  LCD.setCursor(0, 2);
  LCD.print("+   Bitte warten   +");
  LCD.setCursor(0, 3);
  LCD.print("++++++++++++++++++++");
  delay(1000);
  //Display Calendar
  displayCalendar();  // Main flow for drawing calendar

  //Update every 5 Minutes
  delay(300000);
  LCD.noBacklight();
  LCD.clear();
}
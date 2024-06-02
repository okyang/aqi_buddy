#include <Arduino.h>
#include <Adafruit_PM25AQI.h>
#include <SoftwareSerial.h>
#include "TFT_eSPI.h"
#include "Free_Fonts.h"

#define SCREEN_HEIGHT
#define SCREEN_WIDTH

TFT_eSPI tft;
SoftwareSerial pmSerial(2, 3); // pin #2 is IN from sensor (TX pin on sensor), leave pin #3 disconnected
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

void drawGraphics()
{
  tft.begin();
  tft.setRotation(3);

  tft.fillScreen(TFT_RED);        // Red background
  tft.drawPixel(4, 7, TFT_BLACK); // drawing a black pixel at (4,7)
}

void setup()
{
  drawGraphics();

  // -------- Adafruit PMSA003I Air Quality Sensor Setup ----------
  // Wait for serial monitor to open
  Serial.begin(115200);

  Serial.println("Adafruit PMSA003I Air Quality Sensor");

  // Wait one second for sensor to boot up!
  delay(1000);

  // baud rate of the PM2.5 sensor is set
  pmSerial.begin(9600);

  if (!aqi.begin_UART(&pmSerial))
  { // connect to the sensor over software serial
    Serial.println("Could not find PM 2.5 sensor!");
    while (1)
      delay(10);
  }

  Serial.println("PM25 found!");
}

void loop()
{
  PM25_AQI_Data data;

  if (!aqi.read(&data))
  {
    Serial.println("Could not read from AQI");
    delay(500); // try again in a bit!
    return;
  }

  Serial.println();
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Concentration Units (standard)"));
  Serial.println(F("---------------------------------------"));
  Serial.print(F("\t\tPM 2.5: "));
  Serial.print(data.pm25_standard);

  delay(1000);
}

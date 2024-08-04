#include <Arduino.h>
#include <Adafruit_PM25AQI.h>
#include <SoftwareSerial.h>
#include "TFT_eSPI.h"
#include "Free_Fonts.h"

#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
#define TITLE_Y_POSITION 240 * 1 / 7   // title text position of pollution level
#define READING_Y_POSITION 240 * 4 / 5 // title text position of pm2.5 sensor

TFT_eSPI tft;
SoftwareSerial pmSerial(2, 3); // pin #2 is IN from sensor (TX pin on sensor), leave pin #3 disconnected
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
String previousPollutionLevelText = "";

/// @brief Setups screen orientation and tft font
void setupGraphics()
{
  tft.begin();
  tft.setRotation(3);
  tft.setFreeFont(FSSB9); //  Freefont, Sans Serif, Bold, 12pt
  tft.fillScreen(TFT_BLACK);
}

/// @brief Updates the graphics to reflect pm 2.5 readings. AQI level
/// is derived from https://aqicn.org/calculator/.
/// @param pm2p5 Represents the pm 2.5 readings from the sensor. units are ug/m^3
void updateGraphics(int pm2p5)
{
  // air quality color indicator
  String pollutionLevelText;
  int color = TFT_WHITE;
  if (pm2p5 < 0)
  {
    pollutionLevelText = "No AQI Data";
  }
  else if (0 <= pm2p5 && pm2p5 <= 12)
  {
    pollutionLevelText = "AQI is Good";
    color = TFT_GREEN;
  }
  else if (12 < pm2p5 && pm2p5 <= 35)
  {
    pollutionLevelText = "AQI is Moderate";
    color = TFT_GREENYELLOW;
  }
  else if (35 < pm2p5 && pm2p5 <= 56)
  {
    pollutionLevelText = "AQI is Unhealthy for Sensitive";
    color = TFT_YELLOW;
  }
  else
  {
    pollutionLevelText = "AQI is Unhealthy";
    color = TFT_RED;
  }

  // clear text in UI
  if (previousPollutionLevelText != pollutionLevelText)
  {
    tft.fillRect(0, TITLE_Y_POSITION, SCREEN_WIDTH, TITLE_Y_POSITION + 3, TFT_BLACK);
  }
  tft.fillRect(0, READING_Y_POSITION, SCREEN_WIDTH, READING_Y_POSITION + 3, TFT_BLACK);
  previousPollutionLevelText = pollutionLevelText;

  // draw in UI elements
  int pollutionLevelTextWidth = tft.textWidth(pollutionLevelText);
  tft.drawString(pollutionLevelText, (SCREEN_WIDTH - pollutionLevelTextWidth) / 2, TITLE_Y_POSITION);
  tft.fillCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, color);

  // draw aqi level
  String aqiLevelText = "PM2.5 is " + String(pm2p5) + " ug/m^3";
  int aqiLevelTextWidth = tft.textWidth(aqiLevelText);
  tft.drawString(aqiLevelText, (SCREEN_WIDTH - aqiLevelTextWidth) / 2, READING_Y_POSITION);
  // tft.drawString("", (SCREEN_WIDTH - aqiLevelTextWidth) / 2, READING_Y_POSITION);
}

void setup()
{
  setupGraphics();
  updateGraphics(-1);

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
  Serial.print(F("PM 2.5: "));
  Serial.print(data.pm25_standard);

  // Update pm 2.5 graphics
  updateGraphics(data.pm25_standard);
  delay(100);
}

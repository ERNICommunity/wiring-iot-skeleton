/*
 * main.cpp
 *
 *  Created on: 19.05.2016
 *      Author: niklausd
 */

#include <Arduino.h>

// PlatformIO libraries
#include <NTPClient.h>
#include <SerialCommand.h> // pio lib install 173,  lib details see https://github.com/kroimon/Arduino-SerialCommand
#include <SpinTimer.h> // pio lib install 11599, lib details see https://github.com/dniklaus/spin-timer
#include <WiFiUdp.h>

// private libraries
#include <AppHandler.h>
#include <ProductDebug.h>

SerialCommand* sCmd = nullptr;

#if defined(ESP8266) || defined(ESP32)
AppHandler::AppHandler g_appHandler;
#endif

#define LEAP_YEAR(Y) ((Y > 0) && !(Y % 4) && ((Y % 100) || !(Y % 400)))
/**
 * @brief Get the Formatted Date string yyyy-mm-dd
 *
 * @param secs      Seconds since Jan 1, 1970
 * @return String   Formatted date
 */
static String getFormattedDate(uint32_t secs)
{
  // Full days since epoch
  uint32_t rawTime = secs / 86400L;
  uint32_t days = 0, year = 1970;
  uint8_t month = 0;
  static const uint8_t monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Get year and days after year start
  while ((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawTime)
  {
    year++;
  }
  days = rawTime - days + (LEAP_YEAR(year) ? 366 : 365);

  // Get month and days after month start
  for (month = 0; month < 12; month++)
  {
    uint8_t monthLength;
    if (month == 1)
    { // february
      monthLength = LEAP_YEAR(year) ? 29 : 28;
    }
    else
    {
      monthLength = monthDays[month];
    }
    if (days < monthLength)
    {
      break;
    }
    days -= monthLength;
  }

  // Construct oputput
  String monthStr = ++month < 10 ? "0" + String(month) : String(month);
  String dayStr = ++days < 10 ? "0" + String(days) : String(days);
  return String(year) + "-" + monthStr + "-" + dayStr;
}

/**
 * @brief Construct sample telemetry message for DMN IoT Hub
 *
 * @return String   Sample message
 */
static String constructMessage(void)
{
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP);
  timeClient.begin();
  while (timeClient.getEpochTime() < 1000)
  {
    timeClient.update();
    delay(500);
  }

  // Get coordinates from configuration
  const ConfigTypes::sysConfig* configs = g_appHandler.getConfigurations();
  String longitude = String(configs->location.longitude, 13);
  String latitude = String(configs->location.latitude, 13);
  String altitude = String(configs->location.altitude, 13);

  String timeStamp = getFormattedDate(timeClient.getEpochTime()) + " " + timeClient.getFormattedTime() +
                     "+00";
  String pM25 = String(25.0 * random(LONG_MAX) / LONG_MAX, 13);
  String pM100 = String(100.0 * random(LONG_MAX) / LONG_MAX, 13);
  String airTemp = String(100.0 * random(LONG_MAX) / LONG_MAX, 13);
  String relHumid = String(50.0 * random(LONG_MAX) / LONG_MAX, 13);
  String message = "{\"deviceConfig\":{\"longitude\":" + longitude + ",\"latitude\":" + latitude +
                   ",\"altitude\":" + altitude + "},\"dataPoint\":{\"timeStamp\":\"" + timeStamp +
                   "\",\"particulateMatter25\":" + pM25 + ",\"particulateMatter100\":" + pM100 +
                   ",\"airTemperature\":" + airTemp + ",\"relativeHumidity\":" + relHumid + "}}";

  Serial.println("Sending telemetry message: " + message);

  return message;
}

constexpr uint32_t MSG_INTERVAL_MS = 15000;
/**
 * @brief Sample code to send Azure Iot Hub telemetry
 *
 */
class MsgSenderSpinTimerAction : public SpinTimerAction
{
public:
  void timeExpired() override
  {
    uint8_t flag = g_appHandler.sendAzureTelemetry(constructMessage());
    if (flag != AppHandler::SUCCESS)
    {
      Serial.print("Send IoT Hub telemetry failed with code: ");
      Serial.println(flag);
    }
  }
};

void setup()
{
  //-----------------------------------------------------------------------------
  // Debug
  //-----------------------------------------------------------------------------
  setupProdDebugEnv();

  //-----------------------------------------------------------------------------
  // Application
  //-----------------------------------------------------------------------------
  uint8_t flag = g_appHandler.initApp();
  if (flag != AppHandler::SUCCESS)
  {
    Serial.print("Application initialization failed with code: ");
    Serial.println(flag);
  }

  //-----------------------------------------------------------------------------
  // Sample code
  //-----------------------------------------------------------------------------
  new SpinTimer(MSG_INTERVAL_MS, new MsgSenderSpinTimerAction(), SpinTimer::IS_RECURRING,
                SpinTimer::IS_AUTOSTART);
}

void loop()
{
  // file deepcode ignore CppSameEvalBinaryExpressionfalse: sCmd gets instantiated by setupProdDebugEnv()
  if (nullptr != sCmd)
  {
    sCmd->readSerial(); // process serial commands
  }

  scheduleTimers(); // process Timers
  g_appHandler.loopApp();
}

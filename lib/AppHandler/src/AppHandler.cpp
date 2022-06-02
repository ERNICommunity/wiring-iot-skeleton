/*
 * AppHandler.cpp
 *
 *  Created on: 23.05.2022
 *      Author: Armando Amoros
 */

#include "AppHandler.h"

#include <DbgTraceLevel.h>
#include <DbgTracePort.h>

//-----------------------------------------------------------------------------

void setupBuiltInLed()
{
#if defined(ESP8266)
  digitalWrite(LED_BUILTIN, 1); // LED state is inverted on ESP8266
#else
  digitalWrite(LED_BUILTIN, 0);
#endif
}

void setBuiltInLed(bool state)
{
#if defined(ESP8266) || defined(BOARD_LOLIN_D32)
  digitalWrite(LED_BUILTIN,
               !state); // LED state is inverted on ESP8266 & LOLIN D32
#else
  digitalWrite(LED_BUILTIN, state);
#endif
}

ConfigHandler::ConfigHandler AppHandler::AppHandler::s_configHandler;

uint8_t AppHandler::AppHandler::initApp()
{
  uint8_t outFlag = SUCCESS;

  pinMode(LED_BUILTIN, OUTPUT);
  setBuiltInLed(false);

  //-----------------------------------------------------------------------------
  // Little FS file system
  //-----------------------------------------------------------------------------
  FileHandler::initFS();

  //-----------------------------------------------------------------------------
  // Load initial configurations
  //-----------------------------------------------------------------------------
  if (s_configHandler.loadConfigurationFromFile(ConfigHandler::DEFAULT_PATH))
  {
    Serial.println(F("ERROR: Loading configurations failed."));
    outFlag = GENERAL_ERROR;
  }

  //-----------------------------------------------------------------------------
  // Wifi Configuration
  //-----------------------------------------------------------------------------
  if (WifiHandler::initWifi(s_configHandler.getWifiConfig()))
  {
    Serial.println(F("ERROR: Wifi initialization failed."));
    outFlag = GENERAL_ERROR;
  }

  //-----------------------------------------------------------------------------
  // Azure DPS and IoT Hub
  //-----------------------------------------------------------------------------
  if ((WiFi.status() == WL_CONNECTED))
  {
    if (m_azureHandler.azureInit(*s_configHandler.getAzureConfig()))
    {
      Serial.println(F("ERROR: Azure initialization failed."));
    }
  }

  //-----------------------------------------------------------------------------
  // Landing page
  //-----------------------------------------------------------------------------
  if (!s_configHandler.getLandingPageConfig()->disableLandingPage)
  {
    LandingPageHandler::initLandingPage(saveConfigurations, getConfigurations);
  }

  return outFlag;
}

void AppHandler::AppHandler::loopApp()
{
  m_azureHandler.azureLoop();
}

uint8_t AppHandler::AppHandler::saveConfigurations(const ConfigTypes::sysConfig& sysConfig,
                                                   bool makePersisten)
{
  uint8_t outFlag = SUCCESS;

  s_configHandler.setSysConfig(sysConfig, makePersisten);

  return outFlag;
}

const ConfigTypes::sysConfig* AppHandler::AppHandler::getConfigurations(void)
{
  return s_configHandler.getSysConfig();
}

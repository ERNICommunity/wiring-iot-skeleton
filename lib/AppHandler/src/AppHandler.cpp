/*
 * AppHandler.cpp
 *
 *  Created on: 23.05.2022
 *      Author: Armando Amoros
 */

#include "AppHandler.h"

#include <DbgTraceLevel.h>
#include <DbgTracePort.h>
#include <SerialCommand.h>

SerialCommand* sCmd = nullptr;

//-----------------------------------------------------------------------------

void setBuiltInLed(bool state)
{
#if defined(ESP8266) || defined(BOARD_LOLIN_D32)
  // LED state is inverted on ESP8266 & LOLIN D32
  digitalWrite(LED_BUILTIN, static_cast<uint8_t>(!state));
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
  // Debug
  //-----------------------------------------------------------------------------
  m_productDebug.setSaveWifiCrendentialsCallback(saveWifiCredentials);
  m_productDebug.setupProdDebugEnv();

  //-----------------------------------------------------------------------------
  // Little FS file system
  //-----------------------------------------------------------------------------
  FileHandler::initFS();

  //-----------------------------------------------------------------------------
  // Load initial configurations
  //-----------------------------------------------------------------------------
  if (s_configHandler.loadConfigurationFromFile(ConfigHandler::DEFAULT_PATH) != ConfigHandler::SUCCESS)
  {
    Serial.println(F("ERROR: Loading configurations failed."));
    outFlag = GENERAL_ERROR;
  }

  //-----------------------------------------------------------------------------
  // Wifi Configuration
  //-----------------------------------------------------------------------------
  if (m_wifiHandler.initWifi(s_configHandler.getWifiConfig()) != WifiHandler::SUCCESS)
  {
    Serial.println(F("ERROR: Wifi initialization failed."));
    outFlag = GENERAL_ERROR;
  }

  //-----------------------------------------------------------------------------
  // Azure DPS and IoT Hub
  //-----------------------------------------------------------------------------
  if (m_wifiHandler.checkWifiConnection() == WifiHandler::SUCCESS)
  {
    if (m_azureHandler.azureInit(*s_configHandler.getAzureConfig()) != AzureHandler::SUCCESS)
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
  // file deepcode ignore CppSameEvalBinaryExpressionfalse: sCmd gets instantiated by setupProdDebugEnv()
  if (nullptr != sCmd)
  {
    // process serial commands
    sCmd->readSerial();
  }

  if (m_wifiHandler.checkWifiConnection() == WifiHandler::SUCCESS)
  {
    m_azureHandler.azureLoop();
  }
}

uint8_t AppHandler::AppHandler::sendAzureTelemetry(String message) const
{
  uint8_t flag = SUCCESS;
  if (m_azureHandler.sendTelemetry(message) != AzureHandler::SUCCESS)
  {
    flag = GENERAL_ERROR;
  }
  return flag;
}

void AppHandler::AppHandler::saveWifiCredentials(const ConfigTypes::wifiCredentials& wifiCredentials)
{
  ConfigTypes::sysConfig sysConfig = *s_configHandler.getSysConfig();
  sysConfig.wifi.ssid = wifiCredentials.ssid;
  sysConfig.wifi.password = wifiCredentials.password;
  s_configHandler.setSysConfig(sysConfig, true);
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

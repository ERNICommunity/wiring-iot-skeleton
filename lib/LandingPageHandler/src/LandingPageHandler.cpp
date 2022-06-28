/*
 * LandingPageHandler.cpp
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#include "LandingPageHandler.h"

#include "FileHandler.h"

/**
 * @brief Create AsyncWebServer object on port 80
 *
 */
AsyncWebServer* s_server;

const ConfigTypes::sysConfig* s_sysConfig;

void LandingPageHandler::initLandingPage(ConfigTypes::saveConfigCallback_t saveConfigCallback,
                                         ConfigTypes::getConfigCallback_t getConfigCallback)
{
  s_server = new AsyncWebServer(80);
  s_sysConfig = getConfigCallback();
  s_server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(*FileHandler::getFileHandler(), "/index.html", "text/html", false, processor);
  });

  s_server->on("/get", HTTP_GET, [saveConfigCallback](AsyncWebServerRequest* request) {
    if (request->hasParam("reset"))
    {
      ESP.restart();
    }
    else
    {
      ConfigTypes::sysConfig sysConfig{};
      sysConfig.wifi.ssid = request->getParam("ssid")->value();
      sysConfig.wifi.password = request->getParam("password")->value();

      sysConfig.wifi.accessPointSsid = request->getParam("ap_ssid")->value();
      sysConfig.wifi.accessPointPassword = request->getParam("ap_password")->value();
      sysConfig.wifi.deviceStaticIp = request->getParam("ap_staticIp")->value();

      sysConfig.landingPage.disableLandingPage = request->hasParam("lp_dissable");
      sysConfig.landingPage.gpioForLandingPage = request->getParam("lp_gpio")->value().toInt();

      sysConfig.location.latitude = request->getParam("device_latitude")->value().toDouble();
      sysConfig.location.longitude = request->getParam("device_longitude")->value().toDouble();
      sysConfig.location.altitude = request->getParam("device_altitude")->value().toDouble();

      sysConfig.azure.idScope = request->getParam("azure_idScope")->value();
      sysConfig.azure.deviceID = request->getParam("azure_deviceId")->value();
      sysConfig.azure.deviceDerivedKey = request->getParam("azure_deviceDerivedKey")->value();

      saveConfigCallback(sysConfig, true);

      request->send(*FileHandler::getFileHandler(), "/index.html", "text/html", false, processor);
    }
  });
  s_server->serveStatic("/", *FileHandler::getFileHandler(), "/");
  s_server->begin();
}

const String LandingPageHandler::processor(const String& var)
{
  if (static_cast<bool>(var == "SSID"))
  {
    return String(s_sysConfig->wifi.ssid.c_str());
  }
  if (static_cast<bool>(var == "PASSWORD"))
  {
    return String(s_sysConfig->wifi.password.c_str());
  }
  if (static_cast<bool>(var == "AP_SSID"))
  {
    return String(s_sysConfig->wifi.accessPointSsid.c_str());
  }
  if (static_cast<bool>(var == "AP_PASSWORD"))
  {
    return String(s_sysConfig->wifi.accessPointPassword.c_str());
  }
  if (static_cast<bool>(var == "AP_STATICIP"))
  {
    return String(s_sysConfig->wifi.deviceStaticIp.c_str());
  }
  if (static_cast<bool>(var == "LP_DISSABLE"))
  {
    return s_sysConfig->landingPage.disableLandingPage ? "checked" : "";
  }
  if (static_cast<bool>(var == "LP_GPIO"))
  {
    return String(s_sysConfig->landingPage.gpioForLandingPage);
  }
  if (static_cast<bool>(var == "DEVICE_LATITUDE"))
  {
    return String(s_sysConfig->location.latitude, 10);
  }
  if (static_cast<bool>(var == "DEVICE_LONGITUDE"))
  {
    return String(s_sysConfig->location.longitude, 10);
  }
  if (static_cast<bool>(var == "DEVICE_ALTITUDE"))
  {
    return String(s_sysConfig->location.altitude, 10);
  }
  if (static_cast<bool>(var == "AZURE_DEVICEID"))
  {
    return String(s_sysConfig->azure.deviceID.c_str());
  }
  if (static_cast<bool>(var == "AZURE_DEVICEDERIVEDKEY"))
  {
    return String(s_sysConfig->azure.deviceDerivedKey.c_str());
  }
  if (static_cast<bool>(var == "AZURE_IDSCOPE"))
  {
    return String(s_sysConfig->azure.idScope.c_str());
  }

  return String();
}

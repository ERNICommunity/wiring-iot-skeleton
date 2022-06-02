/*
 * WifiHandler.cpp
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#include "WifiHandler.h"

static void enableAccessPoint(const ConfigTypes::wifiCredentials* wifiCredentials)
{
  if (!wifiCredentials->accessPointSsid.isEmpty())
  {
    WiFi.softAP(wifiCredentials->accessPointSsid.c_str(), wifiCredentials->accessPointPassword.c_str());
  }
  if (!wifiCredentials->deviceStaticIp.isEmpty())
  {
    IPAddress ipAddress, subnet;
    ipAddress.fromString(wifiCredentials->deviceStaticIp.c_str());
    subnet.fromString("255.255.255.0");
    WiFi.softAPConfig(ipAddress, ipAddress, subnet);
  }

  Serial.print("Access point IP address: ");
  Serial.println(WiFi.softAPIP());
}

static uint8_t enableWifi(const ConfigTypes::wifiCredentials* wifiCredentials)
{
  uint8_t outFlag = WifiHandler::SUCCESS;
  WiFi.begin(wifiCredentials->ssid.c_str(), wifiCredentials->password.c_str());

  if (wifiCredentials->ssid.length() > 0)
  {
    Serial.print("Attempting to connect to WiFi");
    while (WiFi.status() == WL_DISCONNECTED)
    {
      delay(500);
      Serial.print('.');
    }
    Serial.println();

    if (WiFi.status() != WL_CONNECTED)
    {
      outFlag = WifiHandler::FAIL_CONNECTION;
    }

    Serial.print("WiFi Connection status: ");
    Serial.println(WiFi.status());
    Serial.print("WiFi IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.print("WiFi SSID is empty");
    outFlag = WifiHandler::FAIL_CONNECTION;
  }

  return outFlag;
}

uint8_t WifiHandler::initWifi(const ConfigTypes::wifiCredentials* wifiCredentials)
{
  uint8_t outFlag = SUCCESS;
#if defined(ESP8266)
  enableWiFiAtBootTime();
#endif
#if defined(ESP8266) || defined(ESP32)
  WiFi.persistent(true);
  WiFi.mode(WIFI_AP_STA);
#else
  Serial.println("Wifi for this MC is not yet supported");
  return WIFI_DEVICE_NOT_SUPPORTED;
#endif

  enableAccessPoint(wifiCredentials);
  outFlag = enableWifi(wifiCredentials);

  return outFlag;
}

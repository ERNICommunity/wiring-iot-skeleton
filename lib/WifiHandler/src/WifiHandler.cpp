/*
 * WifiHandler.cpp
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#include "WifiHandler.h"

static void enableAccessPoint(const wifiCredentials *wifiCredentials)
{
    WiFi.softAP(wifiCredentials->accessPointSsid.c_str(), wifiCredentials->accessPointPassword.c_str());

    Serial.print("Access point IP address: ");
    Serial.println(WiFi.softAPIP());
}

static uint8_t enableWifi(const wifiCredentials *wifiCredentials)
{
    uint8_t outFlag = WIFI_SUCCESS;
    WiFi.begin(wifiCredentials->ssid.c_str(), wifiCredentials->password.c_str());

    Serial.print("Attempting to connect to WiFi");
    while (WiFi.status() == WL_DISCONNECTED)
    {
        delay(500);
        Serial.print('.');
    }
    Serial.println();
    if (WiFi.status() != WL_CONNECTED)
    {
        outFlag = WIFI_FAIL_CONNECTION;
    }

    Serial.print("WiFi IP address: ");
    Serial.println(WiFi.localIP());

    return outFlag;
}

uint8_t initWifi(const wifiCredentials *wifiCredentials, WiFiClient **wifiClient)
{
    uint8_t outFlag = WIFI_SUCCESS;
#if defined(ESP8266)
    enableWiFiAtBootTime();
#endif
#if defined(ESP8266) || defined(ESP32)
    WiFi.persistent(true);
    WiFi.mode(WIFI_AP_STA);

    // ESP8266 / ESP32 WiFi Client
    *wifiClient = new WiFiClient();
#else
    Serial.println("Wifi for this MC is not yet supported");
    return WIFI_DEVICE_NOT_SUPPORTED;
#endif

    enableAccessPoint(wifiCredentials);
    outFlag = enableWifi(wifiCredentials);

    return outFlag;
}
/*
 * WifiHandler.h
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#ifndef WIFIHANDLER_H_
#define WIFIHANDLER_H_

#include <stdint.h>
#include <string>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
// see https://github.com/espressif/arduino-esp32/issues/1960#issuecomment-429546528
#endif

// Error codes
constexpr uint8_t WIFI_SUCCESS = 0;
constexpr uint8_t WIFI_FAIL_CONNECTION = 1;
constexpr uint8_t WIFI_DEVICE_NOT_SUPPORTED = 2;

// Configuration sub-structures
#include "configHandler.h"
/* struct wifiCredentials
{
    std::string ssid{};
    std::string password{};
    std::string deviceStaticIp{};
}; */

/**
 * @brief Initialize Wifi module
 *
 * @param wifiCredentials   Wifi credentials
 * @param[out] wifiClient   Double pointer to wifiClient
 * @return uint8_t          Error code. 0 if successful
 */
uint8_t initWifi(const wifiCredentials *wifiCredentials, WiFiClient **wifiClient);

#endif /* WIFIHANDLER_H_ */

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

#include "ConfigTypes.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
// see
// https://github.com/espressif/arduino-esp32/issues/1960#issuecomment-429546528
#endif

namespace WifiHandler
{
// Error codes
constexpr uint8_t SUCCESS = 0;
constexpr uint8_t FAIL_CONNECTION = 1;
constexpr uint8_t DEVICE_NOT_SUPPORTED = 2;
constexpr uint8_t WIFI_DISCONNECTED = 3;

class WifiHandler
{
public:
  WifiHandler() = default;
  virtual ~WifiHandler() = default;

  /**
   * @brief Initialize Wifi module
   *
   * @param wifiCredentials   Wifi credentials
   * @return uint8_t          Error code. 0 if successful
   */
  uint8_t initWifi(const ConfigTypes::wifiCredentials* wifiCredentials);

  /**
   * @brief Check wifi connection and reconnect if necessary
   *
   * @return uint8_t    Error code. 0 if successful
   */
  uint8_t checkWifiConnection();

private:
  WifiHandler(const WifiHandler&);
  WifiHandler& operator=(const WifiHandler&);

  ConfigTypes::wifiCredentials m_wifiCredentials;
};
} // namespace WifiHandler
#endif /* WIFIHANDLER_H_ */

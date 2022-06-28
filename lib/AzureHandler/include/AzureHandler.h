/*
 * AzureHandler.h
 *
 *  Created on: 15.05.2022
 *      Author: Armando Amoros
 */

#ifndef AZUREHANDLER_H_
#define AZUREHANDLER_H_

#include <stdint.h>

#include <WString.h>

#include "ConfigTypes.h"

namespace AzureHandler
{
constexpr uint16_t MQTT_PACKET_SIZE = 1024;
// Azure IoT Central
#define DPS_END_POINT "https://global.azure-devices-provisioning.net/"

// Error codes
constexpr uint8_t SUCCESS = 0;
constexpr uint8_t GENERAL_ERROR = 1;
constexpr uint8_t DEVICE_NOT_REGISTERED = 2;
constexpr uint8_t DEVICE_REGISTRATION_FAILED = 3;
constexpr uint8_t DEVICE_REGISTRATION_OPERATION_FAILED = 4;
constexpr uint8_t NTP_CLIENT_FAILED = 5;
constexpr uint8_t MQTT_CLIENT_CONNECTION_FAILED = 6;
constexpr uint8_t AZURE_HANDLER_NOT_INITIALIZED = 7;
constexpr uint8_t MQTT_PUBLISH_FAILED = 8;

constexpr uint8_t MAX_NUM_REGISTRATION_ATTEMPTS = 5;

enum HttpsActions
{
  POST,
  GET,
  PUT
};

class AzureHandler
{
public:
  AzureHandler() = default;
  virtual ~AzureHandler() = default;

  /**
   * @brief
   *
   * @param config    Configuration to initialize the Azure module
   * @return uint8_t  Error code, 0 if successful
   */
  uint8_t azureInit(const ConfigTypes::azureConfig& config);

  /**
   * @brief Loop running Azure IoT hub messaging
   *
   * @return uint8_t Error code, 0 if successful
   */
  uint8_t azureLoop(void);

  /**
   * @brief Send telemetry to the devices/{deviceId}/messages/events topic
   *
   * @param message   Message to be sent to the IoT Hub Topic
   * @return uint8_t  Error code, 0 if successful
   */
  uint8_t sendTelemetry(String& message) const;

private:
  AzureHandler(const AzureHandler&);
  AzureHandler& operator=(const AzureHandler&);

  /**
   * @brief Defines if Azure module was initialized correctly
   *
   */
  uint8_t m_isDeviceRegistered{false};
};
} // namespace AzureHandler
#endif /* AZUREHANDLER_H_ */

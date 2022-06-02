/*
 * ConfigHandler.h
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#ifndef CONFIGHANDLER_H_
#define CONFIGHANDLER_H_

#include <stdint.h>

#include "ConfigTypes.h"
#include "WString.h"

namespace ConfigHandler
{
constexpr char DEFAULT_PATH[] = "/config.json";

// Error codes
constexpr uint8_t SUCCESS = 0;
constexpr uint8_t FILE_NOT_FOUND = 1;
constexpr uint8_t JSON_READ_FAILED = 2;
constexpr uint8_t UNABLE_TO_OPEN_FILE = 3;
constexpr uint8_t JSON_WRITE_FAILED = 4;

/**
 * @brief Class handling the board and firmware configuration. Contains
 * configuration for:
 *          - Wifi crendentials
 *          - Landing page for configuration management
 *          - Configuration for Azure IoT Hub enrolment.
 *
 */
class ConfigHandler
{
public:
  ConfigHandler() = default;
  ConfigHandler(ConfigTypes::sysConfig sysConfigIn) : m_sysConfig(sysConfigIn){};
  virtual ~ConfigHandler() = default;

  /**
   * @brief   Load configuration into m_config from file
   *
   * @param path      Path of the configurations file
   * @return uint8_t  Error code. 0 if successful
   */
  uint8_t loadConfigurationFromFile(const char* path);

  /**
   * @brief   Save configrations into path from m_config
   *
   * @param path      Path of the configuration file
   * @return uint8_t  Error code. 0 if successful
   */
  uint8_t saveConfigurationToFile(const char* path) const;

  /**
   * @brief Clear m_sysConfig
   *
   */
  void clearConfigurations(void);

  /**
   * @brief Print data in m_sysConfig
   *
   */
  void printConfiguration(void) const;

  /**
   * @brief Get the Wifi Config struct
   *
   * @return const wifiCredentials*
   */
  const ConfigTypes::wifiCredentials* getWifiConfig(void) const
  {
    return &m_sysConfig.wifi;
  }

  /**
   * @brief Get the Landing Page Config struct
   *
   * @return const landingPageConfig*
   */
  const ConfigTypes::landingPageConfig* getLandingPageConfig(void) const
  {
    return &m_sysConfig.landingPage;
  }

  /**
   * @brief Get the Azure Config struct
   *
   * @return const azureConfig*
   */
  const ConfigTypes::azureConfig* getAzureConfig(void) const
  {
    return &m_sysConfig.azure;
  }

  /**
   * @brief Set the Sys Config object and save to file if required
   *
   * @param sysConfig         Configuration to be set
   * @param makePersisten     If true, configuration will be saved to file
   * @return uint8_t          Error code, 0 if successful
   */
  uint8_t setSysConfig(const ConfigTypes::sysConfig& sysConfig, bool makePersisten);

  /**
   * @brief Get the Sys Config object
   *
   * @return const ConfigTypes::sysConfig*
   */
  const ConfigTypes::sysConfig* getSysConfig(void) const;

private:
  ConfigHandler(const ConfigHandler&);
  ConfigHandler& operator=(const ConfigHandler&);

  ConfigTypes::sysConfig m_sysConfig{};
};
} // namespace ConfigHandler

#endif /* CONFIGHANDLER_H_ */

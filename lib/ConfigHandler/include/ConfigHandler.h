/*
 * ConfigHandler.h
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#ifndef CONFIGHANDLER_H_
#define CONFIGHANDLER_H_

#include <string>
#include <stdint.h>
#include "WString.h"

// Error codes
constexpr uint8_t SUCCESS = 0;
constexpr uint8_t FILE_NOT_FOUND = 1;
constexpr uint8_t JSON_READ_FAILED = 2;
constexpr uint8_t UNABLE_TO_OPEN_FILE = 3;
constexpr uint8_t JSON_WRITE_FAILED = 4;

// Configuration sub-structures
struct wifiCredentials
{
    std::string ssid{};
    std::string password{};
    std::string accessPointSsid{};
    std::string accessPointPassword{};
    std::string deviceStaticIp{};
};
typedef struct wifiCredentials wifiCredentials;

struct landingPageConfig
{
    bool disableLandingPage{};
    uint8_t gpioForLandingPage{};
};
typedef struct landingPageConfig landingPageConfig;

struct azureConfig
{
    std::string deviceID{};
    std::string deviceDerivedKey{};
    std::string idScope{};
};
typedef struct azureConfig azureConfig;

struct sysConfig
{
    wifiCredentials wifi;
    landingPageConfig landingPage;
    azureConfig azure;
};
typedef struct sysConfig sysConfig;

/**
 * @brief Class handling the board and firmware configuration. Contains configuration for:
 *          - Wifi crendentials
 *          - Landing page for configuration management
 *          - Configuration for Azure IoT Hub enrolment.
 *
 */
class ConfigHandler
{
private:
    sysConfig m_sysConfig;

public:
    ConfigHandler() : m_sysConfig(){};
    ConfigHandler(sysConfig sysConfigIn) : m_sysConfig(sysConfigIn){};
    /**
     * @brief   Load configuration into m_config from file
     *
     * @param path      Path of the configurations file
     * @return uint8_t  Error code. 0 if successful
     */
    uint8_t loadConfigurationFromFile(const char *path);

    /**
     * @brief   Save configrations into path from m_config
     *
     * @param path      Path of the configuration file
     * @return uint8_t  Error code. 0 if successful
     */
    uint8_t saveConfigurationToFile(const char *path);

    /**
     * @brief Clear m_sysConfig
     *
     */
    void clearConfigurations(void);

    /**
     * @brief Print data in m_sysConfig
     *
     */
    void printConfiguration(void);

    /**
     * @brief Get the Wifi Config struct
     *
     * @return const wifiCredentials*
     */
    const wifiCredentials *getWifiConfig(void)
    {
        return &m_sysConfig.wifi;
    }

    /**
     * @brief Get the Landing Page Config struct
     *
     * @return const landingPageConfig*
     */
    const landingPageConfig *getLandingPageConfig(void)
    {
        return &m_sysConfig.landingPage;
    }

    /**
     * @brief Get the Azure Config struct
     *
     * @return const azureConfig*
     */
    const azureConfig *getAzureConfig(void)
    {
        return &m_sysConfig.azure;
    }

    // TODO, Remove this, shouldn't exist
    sysConfig *getSysConfig(void)
    {
        return &m_sysConfig;
    }
};

#endif /* CONFIGHANDLER_H_ */

/*
 * ConfigTypes.h
 *
 *  Created on: 23.05.2022
 *      Author: Armando Amoros
 */

#ifndef CONFIGTYPES_H_
#define CONFIGTYPES_H_

#include <string>
#include <stdint.h>

namespace ConfigTypes
{
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
} // namespace ConfigTypes

#endif /* CONFIGTYPES_H_ */

/*
 * ConfigTypes.h
 *
 *  Created on: 23.05.2022
 *      Author: Armando Amoros
 */

#ifndef CONFIGTYPES_H_
#define CONFIGTYPES_H_

#include <stdint.h>

#include <WString.h>

namespace ConfigTypes
{
// Configuration sub-structures
struct wifiCredentials
{
  String ssid{};
  String password{};
  String accessPointSsid{};
  String accessPointPassword{};
  String deviceStaticIp{};
};
typedef struct wifiCredentials wifiCredentials;

struct landingPageConfig
{
  bool disableLandingPage{};
  uint8_t gpioForLandingPage{};
};
typedef struct landingPageConfig landingPageConfig;

struct deviceLocationConfig
{
  double latitude{};
  double longitude{};
  double altitude{};
};
typedef struct deviceLocationConfig deviceLocationConfig;

struct azureConfig
{
  String deviceID{};
  String deviceDerivedKey{};
  String idScope{};
};
typedef struct azureConfig azureConfig;

struct sysConfig
{
  wifiCredentials wifi;
  landingPageConfig landingPage;
  deviceLocationConfig location;
  azureConfig azure;
};
typedef struct sysConfig sysConfig;
} // namespace ConfigTypes

#endif /* CONFIGTYPES_H_ */

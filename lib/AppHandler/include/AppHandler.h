/*
 * AppHandler.h
 *
 *  Created on: 23.05.2022
 *      Author: Armando Amoros
 */

#include <stdint.h>

#include "AzureHandler.h"
#include "ConfigHandler.h"
#include "ConfigTypes.h"
#include "FileHandler.h"
#include "LandingPageHandler.h"
#include "WifiHandler.h"

#ifndef APPHANDLER_H_
#define APPHANDLER_H_

namespace AppHandler
{
// Error codes
constexpr uint8_t SUCCESS = 0;
constexpr uint8_t GENERAL_ERROR = 1;

/**
 * @brief Class handling the application modules
 *
 */
class AppHandler
{
public:
  AppHandler() = default;
  virtual ~AppHandler() = default;

  /**
   * @brief Init the application handler
   *
   * @return uint8_t Error code, 0 if successful
   */
  uint8_t initApp(void);

  /**
   * @brief Loop through the app tasks
   *
   */
  void loopApp(void);

  /**
   * @brief Callback function to modify configurations in s_configHandler and save to file
   *
   * @param sysConfig         Configuration to be updated
   * @param makePersisten     Save to internal storage
   * @return uint8_t          Error code, 0 if successful
   */
  static uint8_t saveConfigurations(const ConfigTypes::sysConfig& sysConfig, bool makePersisten);

  /**
   * @brief Get the Configurations callback
   *
   * @return const ConfigTypes::sysConfig*
   */
  static const ConfigTypes::sysConfig* getConfigurations(void);

private:
  AppHandler(const AppHandler&);
  AppHandler& operator=(const AppHandler&);

  static ConfigHandler::ConfigHandler s_configHandler;
  AzureHandler::AzureHandler m_azureHandler;
};

} // namespace AppHandler

#endif /* APPHANDLER_H_ */

/*
 * AppHandler.h
 *
 *  Created on: 23.05.2022
 *      Author: Armando Amoros
 */

#include "ConfigTypes.h"
#include <stdint.h>

#include "ConfigHandler.h"
#include "FileHandler.h"
#include "WifiHandler.h"
#include "LandingPageHandler.h"
#include "AzureHandler.h"

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
    private:
        static ConfigHandler::ConfigHandler m_configHandler;
        AzureHandler::AzureHandler m_azureHandler;

    public:
        /**
         * @brief Init the application handler
         *
         * @return uint8_t Error code, 0 if successful
         */
        uint8_t initAPpp(void);

        /**
         * @brief Loop through the app tasks
         *
         */
        void loopApp(void);

        /**
         * @brief Callback function to modify configurations in m_configHandler and save to file
         *
         * @param sysConfig         Configuration to be updated
         * @param makePersisten     Save to internal storage
         * @return uint8_t          Error code, 0 if successful
         */
        static uint8_t saveConfigurations(const ConfigTypes::sysConfig &sysConfig, bool makePersisten);

        /**
         * @brief Get the Configurations callback
         *
         * @return const ConfigTypes::sysConfig*
         */
        static const ConfigTypes::sysConfig *getConfigurations(void);
    };

} // namespace AppHandler

#endif /* APPHANDLER_H_ */

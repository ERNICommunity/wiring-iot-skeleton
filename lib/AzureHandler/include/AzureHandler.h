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
#define MQTT_PACKET_SIZE 1024
// Azure IoT Central
#define DPS_END_POINT "https://global.azure-devices-provisioning.net/"

    // Error codes
    constexpr uint8_t SUCCESS = 0;
    constexpr uint8_t GENERAL_ERROR = 1;
    constexpr uint8_t DEVICE_NOT_REGISTERED = 2;
    constexpr uint8_t DEVICE_REGISTRATION_FAILED = 3;
    constexpr uint8_t DEVICE_REGISTRATION_OPERATION_FAILED = 4;

    constexpr uint8_t MAX_NUM_REGISTRATION_ATTEMPTS = 5;

    enum HttpsActions
    {
        POST,
        GET,
        PUT
    };

    class AzureHandler
    {
    private:
        /**
         * @brief Defines if Azure module was initialized correctly
         *
         */
        bool m_isDeviceRegistered{false};

    public:
        /**
         * @brief
         *
         * @param config    Configuration to initialize the Azure module
         * @return uint8_t  Error code, 0 if successful
         */
        uint8_t azureInit(const ConfigTypes::azureConfig &config);

        /**
         * @brief   Loop running Azure IoT hub messaging
         *
         */
        void azureLoop(void);
    };
}
#endif /* AZUREHANDLER_H_ */
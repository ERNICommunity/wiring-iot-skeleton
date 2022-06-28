/*
 * LandingPageHandler.h
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#ifndef LANDINGPAGEHANDLER_H_
#define LANDINGPAGEHANDLER_H_

#include "ConfigHandler.h"
#include "ConfigTypes.h"

#if defined(ESP8266)
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <freertos/FreeRTOS.h>

#include "AsyncTCP.h"

#endif
#include <ESPAsyncWebServer.h>

namespace LandingPageHandler
{
/**
 * @brief Initialize web server for landing page
 *
 * @param saveConfigCallback    Callback to save system configs
 * @param setConfigCallback     Callback to load system configs
 */
void initLandingPage(ConfigTypes::saveConfigCallback_t saveConfigCallback,
                     ConfigTypes::getConfigCallback_t getConfigCallback);

/**
 * @brief Function to replace placeholders in HTML
 *
 * @param var       String representing the placeholder
 * @return String   Value for placeholder
 */
const String processor(const String& var);
} // namespace LandingPageHandler

#endif /* LANDINGPAGEHANDLER_H_ */

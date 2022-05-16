/*
 * LandingPageHandler.h
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#ifndef LANDINGPAGEHANDLER_H_
#define LANDINGPAGEHANDLER_H_

#include "ConfigHandler.h"

#if defined(ESP8266)
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

/**
 * @brief Initialize web server for landing page
 *
 * @param sysConfig Configuration to be used
 */
void initLandingPage(sysConfig *sysConfig);

/**
 * @brief Function to replace placeholders in HTML
 *
 * @param var       String representing the placeholder
 * @return String   Value for placeholder
 */
const String processor(const String &var);

#endif /* LANDINGPAGEHANDLER_H_ */

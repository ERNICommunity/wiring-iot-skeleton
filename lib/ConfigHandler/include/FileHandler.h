/*
 * FileHandler.h
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#if defined(ESP8266)
#include <LittleFS.h>
#elif defined(ESP32)
#include <SPIFFS.h>
#endif

/**
 * @brief Initialize the file system
 *
 */
static void initFS()
{
    bool isFsInit = false;
#if defined(ESP8266)
    isFsInit = !LittleFS.begin();
#elif defined(ESP32)
    isFsInit = !SPIFFS.begin();
#else
    isFsInit = false;
    Serial.println("File system for this MC is not yet supported");
#endif
    if (isFsInit)
    {
        Serial.println("An error has occurred while mounting the file system");
    }
    Serial.println("File system mounted successfully");
}

/**
 * @brief Open a file
 *
 * @param path  Path to the file to be opened
 * @param mode  Access rights required
 * @return File File object to the open file
 */
static File FsOpen(const char *path, const char *mode)
{
#if defined(ESP8266)
    return LittleFS.open(path, mode);
#elif defined(ESP32)
    return SPIFFS.open(path, mode);
#else
    Serial.println("File system for this MC is not yet supported");
    return nullptr;
#endif
}

#endif /* FILEHANDLER_H_ */

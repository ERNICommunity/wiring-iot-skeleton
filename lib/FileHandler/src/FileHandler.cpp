/*
 * FileHandler.cpp
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#include "FileHandler.h"

void FileHandler::initFS()
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

File FileHandler::FsOpen(const char* path, const char* mode)
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

fs::FS* FileHandler::getFileHandler(void)
{
#if defined(ESP8266)
  return &LittleFS;
#elif defined(ESP32)
  return &SPIFFS;
#else
  Serial.println("File system for this MC is not yet supported");
  return nullptr;
#endif
}

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

namespace FileHandler
{
/**
 * @brief Initialize the file system
 *
 */
void initFS();

/**
 * @brief Open a file
 *
 * @param path  Path to the file to be opened
 * @param mode  Access rights required
 * @return File File object to the open file
 */
File FsOpen(const char* path, const char* mode);

/**
 * @brief Get the File Handler object
 *
 * @return FS
 */
fs::FS* getFileHandler(void);
} // namespace FileHandler

#endif /* FILEHANDLER_H_ */

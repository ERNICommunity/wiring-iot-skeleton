/*
 * ConfigHandler.cpp
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#include "ConfigHandler.h"
#include <ArduinoJson.h>
#if defined(ESP8266)
#include <LittleFS.h>
#elif defined(ESP32)
#include <SPIFFS.h>
#endif
#include <string>

static std::string assignString(const char *input)
{
    std::string tmp{};

    if (input)
    {
        tmp.assign(input);
    }

    return tmp;
}

uint8_t ConfigHandler::loadConfigurationFromFile(const char *path)
{
    uint8_t outFlag = SUCCESS;
    File configFile = LittleFS.open(path, "r");
    StaticJsonDocument<300> doc;
    // Clear configurations
    clearConfigurations();

    // Check if file exists
    if (!configFile)
    {
        Serial.println("ERROR: FILE_NOT_FOUND");
        outFlag = FILE_NOT_FOUND;
    }

    // It is important the initilization of 'buf' is global to this function in order to save memory when
    // doing the deserialization given the arduino JSON memory managment when passing 'bug' as a char*,
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    // Deserialize JSON
    if (outFlag == SUCCESS)
    {
        // Read file
        configFile.readBytes(buf.get(), size);
        // Parse JSON content
        if (deserializeJson(doc, buf.get()))
        {
            Serial.println("ERROR: JSON_READ_FAILED");
            outFlag = JSON_READ_FAILED;
        }
    }

    // Extract configuration
    if (outFlag == SUCCESS)
    {
        // Wifi
        m_sysConfig.wifi.ssid.assign(assignString(doc["wifi"]["ssid"]));
        m_sysConfig.wifi.password.assign(assignString(doc["wifi"]["password"]));
        // Landing page
        m_sysConfig.landingPage.deviceStaticIp.assign(assignString(doc["landingPage"]["deviceStaticIp"]));
        m_sysConfig.landingPage.disableLandingPage = doc["landingPage"]["disableLandingPage"];
        m_sysConfig.landingPage.gpioForLandingPage = doc["landingPage"]["gpioForLandingPage"];
        // Azure config
        m_sysConfig.azure.deviceID.assign(assignString(doc["azure"]["deviceID"]));
        m_sysConfig.azure.deviceDerivedKey.assign(assignString(doc["azure"]["deviceDerivedKey"]));
        m_sysConfig.azure.idScope.assign(assignString(doc["azure"]["idScope"]));

        configFile.close();

        Serial.println("Configurations loaded successfuly");
    }

    printConfiguration();
    return outFlag;
}

uint8_t ConfigHandler::saveConfigurationToFile(const char *path)
{
    uint8_t outFlag = SUCCESS;
    File configFile = LittleFS.open(path, "w");

    // Check if file exists
    if (!configFile)
    {
        Serial.println("ERROR: UNABLE_TO_OPEN_FILE");
        outFlag = UNABLE_TO_OPEN_FILE;
    }

    if (outFlag == SUCCESS)
    {

        StaticJsonDocument<300> doc;

        // Construct JSON struct
        JsonObject wifiConfig = doc.createNestedObject("wifi");
        wifiConfig["ssid"] = m_sysConfig.wifi.ssid;
        wifiConfig["password"] = m_sysConfig.wifi.password;
        JsonObject landingPageConfig = doc.createNestedObject("landingPage");
        landingPageConfig["deviceStaticIp"] = m_sysConfig.landingPage.deviceStaticIp;
        landingPageConfig["disableLandingPage"] = m_sysConfig.landingPage.disableLandingPage;
        landingPageConfig["gpioForLandingPage"] = m_sysConfig.landingPage.gpioForLandingPage;
        JsonObject azureConfig = doc.createNestedObject("azure");
        azureConfig["deviceID"] = m_sysConfig.azure.deviceID;
        azureConfig["deviceDerivedKey"] = m_sysConfig.azure.deviceDerivedKey;
        azureConfig["idScope"] = m_sysConfig.azure.idScope;

        if (serializeJson(doc, configFile))
        {
            Serial.println("Configurations saved successfuly");
        }
        else
        {
            Serial.println("ERROR: JSON_WRITE_FAILED");
            outFlag = JSON_WRITE_FAILED;
        }

        configFile.close();
    }

    printConfiguration();
    return outFlag;
}

void ConfigHandler::clearConfigurations(void)
{
    sysConfig temp{};
    m_sysConfig = temp;
}

void ConfigHandler::printConfiguration()
{
    Serial.println();
    Serial.println("#################### Configurations ####################");
    Serial.println("Wifi:");
    Serial.print("\tSSID: ");
    Serial.println(m_sysConfig.wifi.ssid.c_str());
    Serial.print("\tPassword: ");
    Serial.println(m_sysConfig.wifi.password.c_str());

    Serial.println("Landing page:");
    Serial.print("\tDevice static ip: ");
    Serial.println(m_sysConfig.landingPage.deviceStaticIp.c_str());
    Serial.print("\tDisable landing page: ");
    Serial.println(m_sysConfig.landingPage.disableLandingPage);
    Serial.print("\tGPIO for landing page: ");
    Serial.println(m_sysConfig.landingPage.gpioForLandingPage);

    Serial.println("Azure:");
    Serial.print("\tDevice id: ");
    Serial.println(m_sysConfig.azure.deviceID.c_str());
    Serial.print("\tDevice derived key: ");
    Serial.println(m_sysConfig.azure.deviceDerivedKey.c_str());
    Serial.print("\tId scope: ");
    Serial.println(m_sysConfig.azure.idScope.c_str());
    Serial.println("########################################################");
}
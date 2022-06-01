/*
 * ConfigHandler.cpp
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#include "ConfigHandler.h"
#include "FileHandler.h"
#include <ArduinoJson.h>

uint8_t ConfigHandler::ConfigHandler::loadConfigurationFromFile(const char *path)
{
    uint8_t outFlag = SUCCESS;
    File configFile = FileHandler::FsOpen(path, "r");
    StaticJsonDocument<300> doc;
    // Clear configurations
    clearConfigurations();

    // Check if file exists
    if (!configFile)
    {
        Serial.println(F("ERROR: FILE_NOT_FOUND"));
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
            Serial.println(F("ERROR: JSON_READ_FAILED"));
            outFlag = JSON_READ_FAILED;
        }
    }

    // Extract configuration
    if (outFlag == SUCCESS)
    {
        // Wifi
        m_sysConfig.wifi.ssid = String(doc["wifi"]["ssid"].as<String>());
        m_sysConfig.wifi.password = String(doc["wifi"]["password"].as<String>());
        m_sysConfig.wifi.accessPointSsid = String(doc["wifi"]["accessPointSsid"].as<String>());
        m_sysConfig.wifi.accessPointPassword = String(doc["wifi"]["accessPointPassword"].as<String>());
        m_sysConfig.wifi.deviceStaticIp = String(doc["wifi"]["deviceStaticIp"].as<String>());
        // Landing page
        m_sysConfig.landingPage.disableLandingPage = doc["landingPage"]["disableLandingPage"];
        m_sysConfig.landingPage.gpioForLandingPage = doc["landingPage"]["gpioForLandingPage"];
        // Azure config
        m_sysConfig.azure.idScope = String(doc["azure"]["idScope"].as<String>());
        m_sysConfig.azure.deviceID = String(doc["azure"]["deviceID"].as<String>());
        m_sysConfig.azure.deviceDerivedKey = String(doc["azure"]["deviceDerivedKey"].as<String>());

        configFile.close();

        Serial.println(F("Configurations loaded successfuly"));
    }

    printConfiguration();
    return outFlag;
}

uint8_t ConfigHandler::ConfigHandler::saveConfigurationToFile(const char *path) const
{
    uint8_t outFlag = SUCCESS;
    File configFile = FileHandler::FsOpen(path, "w");

    // Check if file exists
    if (!configFile)
    {
        Serial.println(F("ERROR: UNABLE_TO_OPEN_FILE"));
        outFlag = UNABLE_TO_OPEN_FILE;
    }

    if (outFlag == SUCCESS)
    {

        StaticJsonDocument<400> doc;

        // Construct JSON struct
        JsonObject wifiConfig = doc.createNestedObject("wifi");
        wifiConfig["ssid"] = m_sysConfig.wifi.ssid;
        wifiConfig["password"] = m_sysConfig.wifi.password;
        wifiConfig["accessPointSsid"] = m_sysConfig.wifi.accessPointSsid;
        wifiConfig["accessPointPassword"] = m_sysConfig.wifi.accessPointPassword;
        wifiConfig["deviceStaticIp"] = m_sysConfig.wifi.deviceStaticIp;
        JsonObject landingPageConfig = doc.createNestedObject("landingPage");
        landingPageConfig["disableLandingPage"] = m_sysConfig.landingPage.disableLandingPage;
        landingPageConfig["gpioForLandingPage"] = m_sysConfig.landingPage.gpioForLandingPage;
        JsonObject azureConfig = doc.createNestedObject("azure");
        azureConfig["idScope"] = m_sysConfig.azure.idScope;
        azureConfig["deviceID"] = m_sysConfig.azure.deviceID;
        azureConfig["deviceDerivedKey"] = m_sysConfig.azure.deviceDerivedKey;

        if (serializeJson(doc, configFile))
        {
            Serial.println(F("Configurations saved successfuly"));
        }
        else
        {
            Serial.println(F("ERROR: JSON_WRITE_FAILED"));
            outFlag = JSON_WRITE_FAILED;
        }

        configFile.close();
    }

    printConfiguration();
    return outFlag;
}

void ConfigHandler::ConfigHandler::clearConfigurations(void)
{
    ConfigTypes::sysConfig temp{};
    m_sysConfig = temp;
}

void ConfigHandler::ConfigHandler::printConfiguration() const
{
    Serial.println();
    Serial.println(F("#################### Configurations ####################"));

    Serial.println(F("Wifi:"));
    Serial.print(F("\tSSID: "));
    Serial.println(m_sysConfig.wifi.ssid.c_str());
    Serial.print(F("\tPassword: "));
    Serial.println(m_sysConfig.wifi.password.c_str());
    Serial.print(F("\tAccess point SSID: "));
    Serial.println(m_sysConfig.wifi.accessPointSsid.c_str());
    Serial.print(F("\tAccess point Password: "));
    Serial.println(m_sysConfig.wifi.accessPointPassword.c_str());
    Serial.print(F("\tDevice static ip: "));
    Serial.println(m_sysConfig.wifi.deviceStaticIp.c_str());

    Serial.println(F("Landing page:"));
    Serial.print(F("\tDisable landing page: "));
    Serial.println(m_sysConfig.landingPage.disableLandingPage);
    Serial.print(F("\tGPIO for landing page: "));
    Serial.println(m_sysConfig.landingPage.gpioForLandingPage);

    Serial.println(F("Azure:"));
    Serial.print(F("\tId scope: "));
    Serial.println(m_sysConfig.azure.idScope.c_str());
    Serial.print(F("\tDevice id: "));
    Serial.println(m_sysConfig.azure.deviceID.c_str());
    Serial.print(F("\tDevice derived key: "));
    Serial.println(m_sysConfig.azure.deviceDerivedKey.c_str());
    Serial.println(F("########################################################"));
}

uint8_t ConfigHandler::ConfigHandler::setSysConfig(const ConfigTypes::sysConfig &sysConfig, bool makePersisten)
{
    uint8_t outFlag = SUCCESS;
    m_sysConfig = sysConfig;

    if (makePersisten)
    {
        outFlag = saveConfigurationToFile(DEFAULT_PATH);
    }

    return outFlag;
}

const ConfigTypes::sysConfig *ConfigHandler::ConfigHandler::getSysConfig() const
{
    return &m_sysConfig;
}

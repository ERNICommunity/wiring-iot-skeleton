/*
 * LandingPageHandler.cpp
 *
 *  Created on: 05.05.2022
 *      Author: Armando Amoros
 */

#include "LandingPageHandler.h"
#include "FileHandler.h"

/**
 * @brief Create AsyncWebServer object on port 80
 *
 */
AsyncWebServer *server;

const ConfigTypes::sysConfig *m_sysConfig;

void LandingPageHandler::initLandingPage(LandingPageHandler::saveConfigCallback_t saveConfigCallback,
                                         LandingPageHandler::getConfigCallback_t getConfigCallback)
{
    server = new AsyncWebServer(80);
    m_sysConfig = getConfigCallback();
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(*FileHandler::getFileHandler(), "/index.html", "text/html", false, processor); });

    server->on("/get", HTTP_GET, [saveConfigCallback](AsyncWebServerRequest *request)
               { if (request->hasParam("reset")) 
                {
                    ESP.restart();
                } else{
                    ConfigTypes::sysConfig sysConfig{};
                    sysConfig.wifi.ssid = request->getParam("ssid")->value();
                    sysConfig.wifi.password = request->getParam("password")->value();

                    sysConfig.wifi.accessPointSsid = request->getParam("ap_ssid")->value();
                    sysConfig.wifi.accessPointPassword = request->getParam("ap_password")->value();
                    sysConfig.wifi.deviceStaticIp = request->getParam("ap_staticIp")->value();

                    sysConfig.landingPage.disableLandingPage = request->hasParam("lp_dissable");
                    sysConfig.landingPage.gpioForLandingPage = request->getParam("lp_gpio")->value().toInt();
                    
                    sysConfig.azure.idScope = request->getParam("azure_idScope")->value();
                    sysConfig.azure.deviceID = request->getParam("azure_deviceId")->value();
                    sysConfig.azure.deviceDerivedKey = request->getParam("azure_deviceDerivedKey")->value();

                    saveConfigCallback(sysConfig, true);

                    request->send(*FileHandler::getFileHandler(), "/index.html", "text/html", false, processor);
                } });
    server->serveStatic("/", *FileHandler::getFileHandler(), "/");
    server->begin();
}

const String LandingPageHandler::processor(const String &var)
{
    if (var == "SSID")
    {
        return String(m_sysConfig->wifi.ssid.c_str());
    }
    if (var == "PASSWORD")
    {
        return String(m_sysConfig->wifi.password.c_str());
    }
    if (var == "AP_SSID")
    {
        return String(m_sysConfig->wifi.accessPointSsid.c_str());
    }
    if (var == "AP_PASSWORD")
    {
        return String(m_sysConfig->wifi.accessPointPassword.c_str());
    }
    if (var == "AP_STATICIP")
    {
        return String(m_sysConfig->wifi.deviceStaticIp.c_str());
    }
    if (var == "LP_DISSABLE")
    {
        return String(m_sysConfig->landingPage.disableLandingPage);
    }
    if (var == "LP_GPIO")
    {
        return String(m_sysConfig->landingPage.gpioForLandingPage);
    }
    if (var == "AZURE_DEVICEID")
    {
        return String(m_sysConfig->azure.deviceID.c_str());
    }
    if (var == "AZURE_DEVICEDERIVEDKEY")
    {
        return String(m_sysConfig->azure.deviceDerivedKey.c_str());
    }
    if (var == "AZURE_IDSCOPE")
    {
        return String(m_sysConfig->azure.idScope.c_str());
    }

    return String();
}
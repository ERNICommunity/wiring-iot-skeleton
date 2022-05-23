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
AsyncWebServer server(80);

const ConfigTypes::sysConfig *m_sysConfig;

void LandingPageHandler::initLandingPage(LandingPageHandler::saveConfigCallback_t saveConfigCallback,
                                         LandingPageHandler::getConfigCallback_t getConfigCallback)
{
    m_sysConfig = getConfigCallback();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(*FileHandler::getFileHandler(), "/index.html", "text/html", false, processor); });

    server.on("/get", HTTP_GET, [&saveConfigCallback](AsyncWebServerRequest *request)
              { if (request->hasParam("reset")) 
                {
                    ESP.restart();
                } else{
                    ConfigTypes::sysConfig sysConfig{};
                    sysConfig.wifi.ssid.assign(request->getParam("ssid")->value().c_str());
                    sysConfig.wifi.password.assign(request->getParam("password")->value().c_str());

                    sysConfig.wifi.accessPointSsid.assign(request->getParam("ap_ssid")->value().c_str());
                    sysConfig.wifi.accessPointPassword.assign(request->getParam("ap_password")->value().c_str());
                    sysConfig.wifi.deviceStaticIp.assign(request->getParam("ap_staticIp")->value().c_str());

                    sysConfig.landingPage.disableLandingPage = request->hasParam("lp_dissable");
                    sysConfig.landingPage.gpioForLandingPage = request->getParam("lp_gpio")->value().toInt();
                    
                    sysConfig.azure.deviceID.assign(request->getParam("azure_deviceId")->value().c_str());
                    sysConfig.azure.deviceDerivedKey.assign(request->getParam("azure_deviceDerivedKey")->value().c_str());
                    sysConfig.azure.idScope.assign(request->getParam("azure_idScope")->value().c_str());

                    saveConfigCallback(sysConfig, true);

                    request->send(*FileHandler::getFileHandler(), "/index.html", "text/html", false, processor);
                } });
    server.serveStatic("/", *FileHandler::getFileHandler(), "/");
    server.begin();
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
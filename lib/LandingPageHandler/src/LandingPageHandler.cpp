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

sysConfig *m_sysConfig = nullptr;

void initLandingPage(sysConfig *sysConfig)
{
    m_sysConfig = sysConfig;

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(*getFileHandler(), "/index.html", "text/html", false, processor); });

    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
              { if (request->hasParam("reset")) 
                {
                    ESP.restart();
                } else{
                    m_sysConfig->wifi.ssid.assign(request->getParam("ssid")->value().c_str());
                    m_sysConfig->wifi.password.assign(request->getParam("password")->value().c_str());

                    m_sysConfig->wifi.accessPointSsid.assign(request->getParam("ap_ssid")->value().c_str());
                    m_sysConfig->wifi.accessPointPassword.assign(request->getParam("ap_password")->value().c_str());
                    m_sysConfig->wifi.deviceStaticIp.assign(request->getParam("ap_staticIp")->value().c_str());

                    if (request->hasParam("lp_dissable")) 
                    {
                        m_sysConfig->landingPage.disableLandingPage = true;
                    }
                    m_sysConfig->landingPage.gpioForLandingPage = request->getParam("lp_gpio")->value().toInt();
                    
                    m_sysConfig->azure.deviceID.assign(request->getParam("azure_deviceId")->value().c_str());
                    m_sysConfig->azure.deviceDerivedKey.assign(request->getParam("azure_deviceDerivedKey")->value().c_str());
                    m_sysConfig->azure.idScope.assign(request->getParam("azure_idScope")->value().c_str());

                    //TODO: This code should not be here, need to find a better way.
                    ConfigHandler tmp(*m_sysConfig);
                    tmp.saveConfigurationToFile("/config.json");
                    request->send(*getFileHandler(), "/index.html", "text/html", false, processor);
                } });
    server.serveStatic("/", *getFileHandler(), "/");
    server.begin();
}

const String processor(const String &var)
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
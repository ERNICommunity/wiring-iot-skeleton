/*
 * AppHandler.cpp
 *
 *  Created on: 23.05.2022
 *      Author: Armando Amoros
 */

#include <AppHandler.h>

#include <ECMqttClient.h> // ERNI Community MQTT client wrapper library (depends on MQTT library)
#include <MqttTopic.h>
#include <ThingSpeak.h> // pio lib install 550,  lib details see https://github.com/mathworks/thingspeak-arduino
#include <DbgTracePort.h>
#include <DbgTraceLevel.h>
#include <LedTestBlinkPublisher.h>

//#define MQTT_SERVER "192.168.43.1"
//#define MQTT_SERVER "iot.eclipse.org"
#define MQTT_SERVER "test.mosquitto.org"
//#define MQTT_SERVER "broker.hivemq.com"

//-----------------------------------------------------------------------------

void setupBuiltInLed()
{
#if defined(ESP8266)
    digitalWrite(LED_BUILTIN, 1); // LED state is inverted on ESP8266
#else
    digitalWrite(LED_BUILTIN, 0);
#endif
}

void setBuiltInLed(bool state)
{
#if defined(ESP8266) || defined(BOARD_LOLIN_D32)
    digitalWrite(LED_BUILTIN, !state); // LED state is inverted on ESP8266 & LOLIN D32
#else
    digitalWrite(LED_BUILTIN, state);
#endif
}

//-----------------------------------------------------------------------------

class TestLedMqttSubscriber : public MqttTopicSubscriber
{
private:
    DbgTrace_Port *m_trPort;

public:
    TestLedMqttSubscriber()
        : MqttTopicSubscriber("test/led"), m_trPort(new DbgTrace_Port("mqttled", DbgTrace_Level::debug))
    {
    }

    ~TestLedMqttSubscriber()
    {
        delete m_trPort;
        m_trPort = 0;
    }

    bool processMessage(MqttRxMsg *rxMsg)
    {
        bool msgHasBeenHandled = false;

        if (0 != rxMsg)
        {
            bool state = atoi(rxMsg->getRxMsgString());
            TR_PRINTF(m_trPort, DbgTrace_Level::debug, "LED state: %s", (state > 0) ? "on" : "off");

            setBuiltInLed(state);

            msgHasBeenHandled = true;
        }
        else
        {
            TR_PRINTF(m_trPort, DbgTrace_Level::error, "rxMsg unavailable!");
        }
        return msgHasBeenHandled;
    }

private:
    // forbidden default functions
    TestLedMqttSubscriber &operator=(const TestLedMqttSubscriber &src); // assignment operator
    TestLedMqttSubscriber(const TestLedMqttSubscriber &src);            // copy constructor
};

//-----------------------------------------------------------------------------

ConfigHandler::ConfigHandler AppHandler::AppHandler::m_configHandler;

uint8_t AppHandler::AppHandler::initAPpp()
{
    uint8_t outFlag = SUCCESS;

    pinMode(LED_BUILTIN, OUTPUT);
    setBuiltInLed(false);

    //-----------------------------------------------------------------------------
    // Little FS file system
    //-----------------------------------------------------------------------------
    FileHandler::initFS();

    //-----------------------------------------------------------------------------
    // Load initial configurations
    //-----------------------------------------------------------------------------
    if (m_configHandler.loadConfigurationFromFile(ConfigHandler::DEFAULT_PATH))
    {
        Serial.println(F("ERROR: Loading configurations failed."));
        outFlag = GENERAL_ERROR;
    }

    //-----------------------------------------------------------------------------
    // Wifi Configuration
    //-----------------------------------------------------------------------------
    if (WifiHandler::initWifi(m_configHandler.getWifiConfig()))
    {
        Serial.println(F("ERROR: Wifi initialization failed."));
        outFlag = GENERAL_ERROR;
    }

    //-----------------------------------------------------------------------------
    // Azure DPS and IoT Hub
    //-----------------------------------------------------------------------------
    if ((WiFi.status() == WL_CONNECTED))
    {
        if (m_azureHandler.azureInit(*m_configHandler.getAzureConfig()))
        {
            Serial.println(F("ERROR: Azure initialization failed."));
        }
    }

    //-----------------------------------------------------------------------------
    // Landing page
    //-----------------------------------------------------------------------------
    if (!m_configHandler.getLandingPageConfig()->disableLandingPage)
    {
        LandingPageHandler::initLandingPage(saveConfigurations, getConfigurations);
    }

    /* #if defined(ESP8266) || defined(ESP32)
        //-----------------------------------------------------------------------------
        // ThingSpeak Client
        //-----------------------------------------------------------------------------
        WiFiClient *wifiClient = new WiFiClient();
        ThingSpeak.begin(*wifiClient);

        //-----------------------------------------------------------------------------
        // MQTT Client
        //-----------------------------------------------------------------------------
        ECMqttClient.begin(MQTT_SERVER);
        new TestLedMqttSubscriber();
        new DefaultMqttSubscriber("test/startup/#");
        new MqttTopicPublisher("test/startup", WiFi.macAddress().c_str(), MqttTopicPublisher::DO_AUTO_PUBLISH);
        new LedTestBlinkPublisher();
    #endif */

    return outFlag;
}

void AppHandler::AppHandler::loopApp()
{
    // ECMqttClient.loop(); // process MQTT Client

    m_azureHandler.azureLoop();
}

uint8_t AppHandler::AppHandler::saveConfigurations(const ConfigTypes::sysConfig &sysConfig, bool makePersisten)
{
    uint8_t outFlag = SUCCESS;

    m_configHandler.setSysConfig(sysConfig, makePersisten);

    return outFlag;
}

const ConfigTypes::sysConfig *AppHandler::AppHandler::getConfigurations(void)
{
    return m_configHandler.getSysConfig();
}

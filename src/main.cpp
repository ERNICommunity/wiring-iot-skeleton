/*
 * main.cpp
 *
 *  Created on: 19.05.2016
 *      Author: niklausd
 */

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
// see https://github.com/espressif/arduino-esp32/issues/1960#issuecomment-429546528
#endif

// PlatformIO libraries
#include <SerialCommand.h>  // pio lib install 173,  lib details see https://github.com/kroimon/Arduino-SerialCommand
#include <ThingSpeak.h>     // pio lib install 550,  lib details see https://github.com/mathworks/thingspeak-arduino
#include <SpinTimer.h>      // pio lib install 11599, lib details see https://github.com/dniklaus/spin-timer
#include <DbgTracePort.h>
#include <DbgTraceLevel.h>


// private libraries
#include <ECMqttClient.h>   // ERNI Community MQTT client wrapper library (depends on MQTT library)
#include <MqttTopic.h>
#include <ProductDebug.h>
#include <LedTestBlinkPublisher.h>

//#define MQTT_SERVER "192.168.43.1"
//#define MQTT_SERVER "iot.eclipse.org"
#define MQTT_SERVER "test.mosquitto.org"
//#define MQTT_SERVER "broker.hivemq.com"

SerialCommand* sCmd = 0;

#if defined(ESP8266) || defined(ESP32)
WiFiClient* wifiClient = 0;
#endif

class TestLedMqttSubscriber : public MqttTopicSubscriber
{
private:
  DbgTrace_Port* m_trPort;
public:
  TestLedMqttSubscriber()
  : MqttTopicSubscriber("test/led")
  , m_trPort(new DbgTrace_Port("mqttled", DbgTrace_Level::debug))
  { }

  ~TestLedMqttSubscriber()
  {
    delete m_trPort;
    m_trPort = 0;
  }

  bool processMessage()
  {
    bool msgHasBeenHandled = false;
    MqttRxMsg* rxMsg = getRxMsg();

    if (isMyTopic())
    {
      if (0 != rxMsg)
      {
        // take responsibility
        bool pinState = atoi(rxMsg->getRxMsgString());
        TR_PRINTF(m_trPort, DbgTrace_Level::debug, "LED state: %s", (pinState > 0) ? "on" : "off");
#if defined(ESP8266)
        digitalWrite(LED_BUILTIN, !pinState);  // LED state is inverted on ESP8266
#else
        digitalWrite(LED_BUILTIN, pinState);
#endif
        msgHasBeenHandled = true;
      }
      else
      {
        TR_PRINTF(m_trPort, DbgTrace_Level::error, "rxMsg unavailable!");
      }
    }
    return msgHasBeenHandled;
  }

private:
  // forbidden default functions
  TestLedMqttSubscriber& operator = (const TestLedMqttSubscriber& src); // assignment operator
  TestLedMqttSubscriber(const TestLedMqttSubscriber& src);              // copy constructor
};

//-----------------------------------------------------------------------------

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  // set LED off
#if defined(ESP8266)
  digitalWrite(LED_BUILTIN, 1);  // LED state is inverted on ESP8266
#else
  digitalWrite(LED_BUILTIN, 0);
#endif

  setupProdDebugEnv();

#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);

  //-----------------------------------------------------------------------------
  // ESP8266 / ESP32 WiFi Client
  //-----------------------------------------------------------------------------
  wifiClient = new WiFiClient();

  //-----------------------------------------------------------------------------
  // ThingSpeak Client
  //-----------------------------------------------------------------------------
  ThingSpeak.begin(*(wifiClient));

  //-----------------------------------------------------------------------------
  // MQTT Client
  //-----------------------------------------------------------------------------
  ECMqttClient.begin(MQTT_SERVER);
  new TestLedMqttSubscriber();
  new DefaultMqttSubscriber("test/startup/#");
  new MqttTopicPublisher("test/startup", WiFi.macAddress().c_str(), MqttTopicPublisher::DO_AUTO_PUBLISH);
  new LedTestBlinkPublisher();
#endif
}

void loop()
{
  // file deepcode ignore CppSameEvalBinaryExpressionfalse: sCmd gets instantiated by setupProdDebugEnv()
  if (0 != sCmd)
  {
    sCmd->readSerial();           // process serial commands
  }
  ECMqttClient.loop();            // process MQTT Client
  scheduleTimers();               // process Timers
}

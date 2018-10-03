/*
 * main.cpp
 *
 *  Created on: 19.05.2016
 *      Author: niklausd
 */

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#endif

// PlatformIO libraries
#include <MQTT.h>           // pio lib install 617,  lib details see https://github.com/256dpi/arduino-mqtt
#include <PubSubClient.h>   // pio lib install 89,   lib details see https://github.com/knolleary/PubSubClient
#include <SerialCommand.h>  // pio lib install 173,  lib details see https://github.com/kroimon/Arduino-SerialCommand
#include <ThingSpeak.h>     // pio lib install 550,  lib details see https://github.com/mathworks/thingspeak-arduino
#include <ArduinoJson.h>    // pio lib install 64,   lib details see https://github.com/bblanchon/ArduinoJson
#include <Timer.h>          // pio lib install 1699, lib details see https://github.com/dniklaus/wiring-timer


// private libraries
#include <DbgCliNode.h>
#include <DbgCliTopic.h>
#include <DbgCliCommand.h>
#include <DbgTracePort.h>
#include <DbgTraceContext.h>
#include <DbgTraceOut.h>
#include <DbgPrintConsole.h>
#include <DbgTraceLevel.h>
#include <ECMqttClient.h>
#include <MqttTopic.h>
#include <string.h>
#include <AppDebug.h>
#include <ProductDebug.h>
#include <RamUtils.h>
#include <LedTestBlinkPublisher.h>

#define MQTT_SERVER "iot.eclipse.org"
//#define MQTT_SERVER "test.mosquitto.org"
//#define MQTT_SERVER "broker.hivemq.com"

SerialCommand* sCmd = 0;

#ifdef ESP8266
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
        digitalWrite(LED_BUILTIN, !pinState);  // LED state is inverted on ESP8266
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
  digitalWrite(LED_BUILTIN, 1);

  setupProdDebugEnv();

#ifdef ESP8266
  WiFi.mode(WIFI_STA);

  //-----------------------------------------------------------------------------
  // ESP8266 WiFi Client
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
  if (0 != sCmd)
  {
    sCmd->readSerial();           // process serial commands
  }
  ECMqttClient.loop();            // process MQTT Client
  scheduleTimers();               // process Timers
}

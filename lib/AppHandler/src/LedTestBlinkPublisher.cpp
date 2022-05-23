/*
 * LedTestBlinkPublisher.cpp
 *
 *  Created on: 01.06.2017
 *      Author: nid
 */

#include <SpinTimer.h>
#include <DbgCliCommand.h>
#include <DbgCliTopic.h>
#include <DbgTracePort.h>
#include <DbgTraceLevel.h>
#include <MqttClientController.h>
#include <LedTestBlinkPublisher.h>

class DbgCli_Cmd_LedBlinkPublisherEn : public DbgCli_Command
{
private:
  LedTestBlinkPublisher* m_ledBlinkPublisher;
public:
  DbgCli_Cmd_LedBlinkPublisherEn(DbgCli_Topic* ledBlinkPublisherTopic, LedTestBlinkPublisher* ledBlinkPublisher)
  : DbgCli_Command(ledBlinkPublisherTopic, "en", "Enable LED Test blink publisher.")
  , m_ledBlinkPublisher(ledBlinkPublisher)
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc - idxToFirstArgToHandle > 0)
    {
      printUsage();
    }
    else
    {
      if (0 != m_ledBlinkPublisher)
      {
        m_ledBlinkPublisher->getTimer()->start(2000);
        TR_PRINTF(MqttClientController::Instance()->trPort(), DbgTrace_Level::alert, "LED Blink publisher enabled");
      }
    }
  }

  void printUsage()
  {
    TR_PRINTF(MqttClientController::Instance()->trPort(), DbgTrace_Level::alert, "%s",  getHelpText());
    TR_PRINTF(MqttClientController::Instance()->trPort(), DbgTrace_Level::alert, "Usage: %s %s %s", 
              DbgCli_Node::RootNode()->getNodeName(), this->getParentNode()->getNodeName(), this->getNodeName());
  }
};

//-----------------------------------------------------------------------------

class DbgCli_Cmd_LedBlinkPublisherDis : public DbgCli_Command
{
private:
  LedTestBlinkPublisher* m_ledBlinkPublisher;
public:
  DbgCli_Cmd_LedBlinkPublisherDis(DbgCli_Topic* ledBlinkPublisherTopic, LedTestBlinkPublisher* ledBlinkPublisher)
  : DbgCli_Command(ledBlinkPublisherTopic, "dis", "Disable LED Test blink publisher.")
  , m_ledBlinkPublisher(ledBlinkPublisher)
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc - idxToFirstArgToHandle > 0)
    {
      printUsage();
    }
    else
    {
      if (0 != m_ledBlinkPublisher)
      {
        m_ledBlinkPublisher->getTimer()->cancel();
        TR_PRINTF(MqttClientController::Instance()->trPort(), DbgTrace_Level::alert, "LED Blink publisher disabled");
      }
    }
  }

  void printUsage()
  {
    TR_PRINTF(MqttClientController::Instance()->trPort(), DbgTrace_Level::alert, "%s",  getHelpText());
    TR_PRINTF(MqttClientController::Instance()->trPort(), DbgTrace_Level::alert, "Usage: %s %s %s", 
              DbgCli_Node::RootNode()->getNodeName(), this->getParentNode()->getNodeName(), this->getNodeName());
  }
};

//-----------------------------------------------------------------------------

class BlinkTimerAction : public SpinTimerAction
{
private:
  LedTestBlinkPublisher* m_ledTestBlinkPublisher;
public:
  BlinkTimerAction(LedTestBlinkPublisher* ledTestBlinkPublisher)
  : m_ledTestBlinkPublisher(ledTestBlinkPublisher)
  { }

  void timeExpired()
  {
    if (0 != m_ledTestBlinkPublisher)
    {
      m_ledTestBlinkPublisher->toggle();
    }
  }
};

LedTestBlinkPublisher::LedTestBlinkPublisher()
: MqttTopicPublisher("test/led", "0")
, m_blinkTimer(new SpinTimer(0, new BlinkTimerAction(this), SpinTimer::IS_RECURRING, SpinTimer::IS_NON_AUTOSTART))
, m_toggle(false)
, m_ledBlinkPublisherTopic(new DbgCli_Topic(DbgCli_Node::RootNode(), "ledpub", "Led Test Blink Publisher debug commands"))
, m_ledBlinkPublisherEnCmd(new DbgCli_Cmd_LedBlinkPublisherEn(m_ledBlinkPublisherTopic, this))
, m_ledBlinkPublisherDisCmd(new DbgCli_Cmd_LedBlinkPublisherDis(m_ledBlinkPublisherTopic, this))
{ }

LedTestBlinkPublisher::~LedTestBlinkPublisher()
{
  delete m_ledBlinkPublisherDisCmd;
  m_ledBlinkPublisherDisCmd = 0;

  delete m_ledBlinkPublisherEnCmd;
  m_ledBlinkPublisherEnCmd = 0;

  delete m_ledBlinkPublisherTopic;
  m_ledBlinkPublisherTopic = 0;

  delete m_blinkTimer->action();
  m_blinkTimer->attachAction(0);

  delete m_blinkTimer;
  m_blinkTimer = 0;
}

void LedTestBlinkPublisher::toggle()
{
  m_toggle = !m_toggle;
  publish(m_toggle ? "1" : "0");
}

SpinTimer* LedTestBlinkPublisher::getTimer()
{
  return m_blinkTimer;
}

//-----------------------------------------------------------------------------


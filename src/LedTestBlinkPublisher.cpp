/*
 * LedTestBlinkPublisher.cpp
 *
 *  Created on: 01.06.2017
 *      Author: nid
 */

#include <Timer.h>
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
        m_ledBlinkPublisher->getTimer()->startTimer(2000);
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
        m_ledBlinkPublisher->getTimer()->cancelTimer();
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

class BlinkTimerAdapter : public TimerAdapter
{
private:
  LedTestBlinkPublisher* m_ledTestBlinkPublisher;
public:
  BlinkTimerAdapter(LedTestBlinkPublisher* ledTestBlinkPublisher)
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
, m_blinkTimer(new Timer(new BlinkTimerAdapter(this), Timer::IS_RECURRING))
, m_toggle(false)
{
  DbgCli_Topic* ledBlinkPublisherTopic = new DbgCli_Topic(DbgCli_Node::RootNode(), "ledpub", "Led Test Blink Publisherdebug commands");
  new DbgCli_Cmd_LedBlinkPublisherEn(ledBlinkPublisherTopic, this);
  new DbgCli_Cmd_LedBlinkPublisherDis(ledBlinkPublisherTopic, this);
}

LedTestBlinkPublisher::~LedTestBlinkPublisher()
{
  delete m_blinkTimer;
  m_blinkTimer = 0;
}

void LedTestBlinkPublisher::toggle()
{
  m_toggle = !m_toggle;
  publish(m_toggle ? "1" : "0");
}

Timer* LedTestBlinkPublisher::getTimer()
{
  return m_blinkTimer;
}

//-----------------------------------------------------------------------------


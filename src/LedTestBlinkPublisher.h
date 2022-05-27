/*
 * LedTestBlinkPublisher.h
 *
 *  Created on: 01.06.2017
 *      Author: nid
 */

#ifndef SRC_LEDTESTBLINKPUBLISHER_H_
#define SRC_LEDTESTBLINKPUBLISHER_H_

#include <MqttTopic.h>

class SpinTimer;
class DbgCli_Topic;
class DbgCli_Command;
class DbgTrace_Port;

class LedTestBlinkPublisher: public MqttTopicPublisher
{
public:
  LedTestBlinkPublisher();
  virtual ~LedTestBlinkPublisher();
  void toggle();
  SpinTimer* getTimer();

private:
  SpinTimer* m_blinkTimer;
  bool m_toggle;
  DbgCli_Topic*   m_ledBlinkPublisherTopic;
  DbgCli_Command* m_ledBlinkPublisherEnCmd;
  DbgCli_Command* m_ledBlinkPublisherDisCmd;
  DbgTrace_Port*  m_trPort;
};

#endif /* SRC_LEDTESTBLINKPUBLISHER_H_ */



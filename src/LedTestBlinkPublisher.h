/*
 * LedTestBlinkPublisher.h
 *
 *  Created on: 01.06.2017
 *      Author: nid
 */

#ifndef SRC_LEDTESTBLINKPUBLISHER_H_
#define SRC_LEDTESTBLINKPUBLISHER_H_

#include <MqttTopic.h>

class Timer;
class DbgCli_Command;

class LedTestBlinkPublisher: public MqttTopicPublisher
{
public:
  LedTestBlinkPublisher();
  virtual ~LedTestBlinkPublisher();
  void toggle();
  Timer* getTimer();

private:
  Timer* m_blinkTimer;
  bool m_toggle;
  DbgCli_Command* m_dbgCmdEn;
  DbgCli_Command* m_dbgCmdDis;
};

#endif /* SRC_LEDTESTBLINKPUBLISHER_H_ */



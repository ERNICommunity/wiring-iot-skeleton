/*
 * main.cpp
 *
 *  Created on: 19.05.2016
 *      Author: niklausd
 */

#include <Arduino.h>

// PlatformIO libraries
#include <SerialCommand.h> // pio lib install 173,  lib details see https://github.com/kroimon/Arduino-SerialCommand
#include <SpinTimer.h>     // pio lib install 11599, lib details see https://github.com/dniklaus/spin-timer

// private libraries
#include <ProductDebug.h>
#include <AppHandler.h>

SerialCommand *sCmd = 0;

#if defined(ESP8266) || defined(ESP32)
AppHandler::AppHandler appHandler;
#endif

void setup()
{
  //-----------------------------------------------------------------------------
  // Debug
  //-----------------------------------------------------------------------------
  setupProdDebugEnv();

  //-----------------------------------------------------------------------------
  // Application
  //-----------------------------------------------------------------------------
  uint8_t flag = appHandler.initAPpp();
  if (flag)
  {
    Serial.print("Application initialization failed with code: ");
    Serial.println(flag);
  }
}

void loop()
{
  // file deepcode ignore CppSameEvalBinaryExpressionfalse: sCmd gets instantiated by setupProdDebugEnv()
  if (0 != sCmd)
  {
    sCmd->readSerial(); // process serial commands
  }

  scheduleTimers(); // process Timers
  appHandler.loopApp();
}

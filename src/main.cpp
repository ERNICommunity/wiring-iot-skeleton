/*
 * main.cpp
 *
 *  Created on: 19.05.2016
 *      Author: niklausd
 */

#include <Arduino.h>

// private libraries
#include <AppHandler.h>

AppHandler::AppHandler g_appHandler;

void setup()
{
  //-----------------------------------------------------------------------------
  // Application
  //-----------------------------------------------------------------------------
  uint8_t flag = g_appHandler.initApp();
  if (flag != AppHandler::SUCCESS)
  {
    Serial.print("Application initialization failed with code: ");
    Serial.println(flag);
  }
}

void loop()
{
  g_appHandler.loopApp();
}

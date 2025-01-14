/*
 * ProductDebug.cpp
 *
 *  Created on: 14.06.2016
 *      Author: nid
 */

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <AppDebug.h>
#include <DbgCliCommand.h>
#include <DbgCliNode.h>
#include <DbgCliTopic.h>
#include <DbgTraceLevel.h>
#include <DbgTracePort.h>

#include "ProductDebug.h"

//-----------------------------------------------------------------------------
// WiFi Commands
//-----------------------------------------------------------------------------
class DbgCli_Cmd_WifiMac : public DbgCli_Command
{
public:
  explicit DbgCli_Cmd_WifiMac(DbgCli_Topic* wifiTopic) :
      DbgCli_Command(wifiTopic, "mac", "Print MAC Address.")
  {
  }
  ~DbgCli_Cmd_WifiMac() override = default;

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle) override
  {
    Serial.println();
    Serial.print("Wifi MAC: ");
    Serial.println(WiFi.macAddress().c_str());
    Serial.println();
  }

private:
  DbgCli_Cmd_WifiMac(const DbgCli_Cmd_WifiMac&);
  DbgCli_Cmd_WifiMac(const DbgCli_Cmd_WifiMac&&);
  DbgCli_Cmd_WifiMac& operator=(const DbgCli_Cmd_WifiMac&);
  DbgCli_Cmd_WifiMac& operator=(const DbgCli_Cmd_WifiMac&&);
};

class DbgCli_Cmd_WifiNets : public DbgCli_Command
{
public:
  explicit DbgCli_Cmd_WifiNets(DbgCli_Topic* wifiTopic) :
      DbgCli_Command(wifiTopic, "nets", "Print nearby networks.")
  {
  }
  ~DbgCli_Cmd_WifiNets() override = default;

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle) override
  {
    bool bailOut = false;

    // scan for nearby networks:
    Serial.println();
    Serial.println("** Scan Networks **");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1)
    {
      Serial.println("Couldn't get a wifi connection");
      bailOut = true;
    }

    if (!bailOut)
    {
      // print the list of networks seen:
      Serial.print("number of available networks:");
      Serial.println(numSsid);

      // print the network number and name for each network found:
      for (int thisNet = 0; thisNet < numSsid; thisNet++)
      {
        Serial.print(thisNet);
        Serial.print(") ");
        Serial.print(WiFi.SSID(thisNet));
        Serial.print(" - Signal: ");
        Serial.print(WiFi.RSSI(thisNet));
        Serial.print(" dBm");
        Serial.print(" - Encryption: ");
        printEncryptionType(WiFi.encryptionType(thisNet));
      }
    }
    Serial.println();
  }

private:
  DbgCli_Cmd_WifiNets(const DbgCli_Cmd_WifiNets&);
  DbgCli_Cmd_WifiNets(const DbgCli_Cmd_WifiNets&&);
  DbgCli_Cmd_WifiNets& operator=(const DbgCli_Cmd_WifiNets&);
  DbgCli_Cmd_WifiNets& operator=(const DbgCli_Cmd_WifiNets&&);

  void printEncryptionType(int thisType)
  {
    // read the encryption type and print out the name:
    switch (thisType)
    {
#if !defined(ESP32)
        // TODO: solve this for ESP32!
      case ENC_TYPE_WEP:
        Serial.println("WEP");
        break;
      case ENC_TYPE_TKIP:
        Serial.println("WPA");
        break;
      case ENC_TYPE_CCMP:
        Serial.println("WPA2");
        break;
      case ENC_TYPE_NONE:
        Serial.println("None");
        break;
      case ENC_TYPE_AUTO:
        Serial.println("Auto");
        break;
#endif
      default:
        Serial.println("Unknown");
        break;
    }
  }
};

class DbgCli_Cmd_WifiStat : public DbgCli_Command
{
public:
  explicit DbgCli_Cmd_WifiStat(DbgCli_Topic* wifiTopic) :
      DbgCli_Command(wifiTopic, "stat", "Show WiFi connection status.")
  {
  }
  ~DbgCli_Cmd_WifiStat() override = default;

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle) override
  {
    wl_status_t wlStatus = WiFi.status();
    Serial.println();
    Serial.println(wlStatus == WL_NO_SHIELD         ? "NO_SHIELD      "
                   : wlStatus == WL_IDLE_STATUS     ? "IDLE_STATUS    "
                   : wlStatus == WL_NO_SSID_AVAIL   ? "NO_SSID_AVAIL  "
                   : wlStatus == WL_SCAN_COMPLETED  ? "SCAN_COMPLETED "
                   : wlStatus == WL_CONNECTED       ? "CONNECTED      "
                   : wlStatus == WL_CONNECT_FAILED  ? "CONNECT_FAILED "
                   : wlStatus == WL_CONNECTION_LOST ? "CONNECTION_LOST"
                   : wlStatus == WL_DISCONNECTED    ? "DISCONNECTED   "
                                                    : "UNKNOWN");
    Serial.println();
    WiFi.printDiag(Serial);
    Serial.println();
  }

private:
  DbgCli_Cmd_WifiStat(const DbgCli_Cmd_WifiStat&);
  DbgCli_Cmd_WifiStat(const DbgCli_Cmd_WifiStat&&);
  DbgCli_Cmd_WifiStat& operator=(const DbgCli_Cmd_WifiStat&);
  DbgCli_Cmd_WifiStat& operator=(const DbgCli_Cmd_WifiStat&&);
};

class DbgCli_Cmd_WifiDis : public DbgCli_Command
{
public:
  explicit DbgCli_Cmd_WifiDis(DbgCli_Topic* wifiTopic) : DbgCli_Command(wifiTopic, "dis", "Disconnect WiFi.")
  {
  }
  ~DbgCli_Cmd_WifiDis() override = default;

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle) override
  {
    Serial.println();
    if (argc - idxToFirstArgToHandle > 0)
    {
      printUsage();
    }
    else
    {
      const bool DO_NOT_SET_wifioff = false;
      WiFi.disconnect(DO_NOT_SET_wifioff);
      Serial.println("WiFi is disconnected now.");
    }
    Serial.println();
  }

  void printUsage()
  {
    Serial.println(getHelpText());
    Serial.println("Usage: dbg wifi dis");
  }

private:
  DbgCli_Cmd_WifiDis(const DbgCli_Cmd_WifiDis&);
  DbgCli_Cmd_WifiDis(const DbgCli_Cmd_WifiDis&&);
  DbgCli_Cmd_WifiDis& operator=(const DbgCli_Cmd_WifiDis&);
  DbgCli_Cmd_WifiDis& operator=(const DbgCli_Cmd_WifiDis&&);
};

class DbgCli_Cmd_WifiCon : public DbgCli_Command
{
public:
  explicit DbgCli_Cmd_WifiCon(DbgCli_Topic* wifiTopic,
                              ConfigTypes::saveWifiCredentialsCallback_t saveWifiCredentialsCallback) :
      DbgCli_Command(wifiTopic, "con", "Connect to WiFi."),
      saveWifiCredentials(saveWifiCredentialsCallback)
  {
  }
  ~DbgCli_Cmd_WifiCon() override = default;

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle) override
  {
    Serial.println();
    uint8_t argsToHandle = argc - idxToFirstArgToHandle;
    if (argsToHandle < 2 || argsToHandle > 3)
    {
      printUsage();
    }
    else
    {
      const char* ssid = args[idxToFirstArgToHandle];
      const char* pass = args[idxToFirstArgToHandle + 1];
      Serial.print("SSID: ");
      Serial.print(ssid);
      Serial.print(", pass: ");
      Serial.println(pass);
      WiFi.begin(ssid, pass);
      Serial.println("WiFi is connecting now.");
      if (argsToHandle == 3 && saveWifiCredentials != nullptr &&
          strcmp(args[idxToFirstArgToHandle + 2], "true") == 0)
      {
        ConfigTypes::wifiCredentials wifiCredentials;
        wifiCredentials.ssid = ssid;
        wifiCredentials.password = pass;
        saveWifiCredentials(wifiCredentials);
      }
    }
    Serial.println();
  }

  void printUsage()
  {
    Serial.println(getHelpText());
    Serial.println("Usage: dbg wifi con <SSID> <passwd> <persistent [optional]>");
  }

private:
  DbgCli_Cmd_WifiCon(const DbgCli_Cmd_WifiCon&);
  DbgCli_Cmd_WifiCon(const DbgCli_Cmd_WifiCon&&);
  DbgCli_Cmd_WifiCon& operator=(const DbgCli_Cmd_WifiCon&);
  DbgCli_Cmd_WifiCon& operator=(const DbgCli_Cmd_WifiCon&&);

  ConfigTypes::saveWifiCredentialsCallback_t saveWifiCredentials{nullptr};
};

//-----------------------------------------------------------------------------

void ProductDebug::ProductDebug::setupProdDebugEnv()
{
  setupDebugEnv();

  //-----------------------------------------------------------------------------
  // WiFi Commands
  //-----------------------------------------------------------------------------
#if defined(ESP8266) || defined(ESP32)
  DbgCli_Topic* wifiTopic = new DbgCli_Topic(DbgCli_Node::RootNode(), "wifi", "WiFi debug commands");
  new DbgCli_Cmd_WifiMac(wifiTopic);
  new DbgCli_Cmd_WifiNets(wifiTopic);
  new DbgCli_Cmd_WifiStat(wifiTopic);
  new DbgCli_Cmd_WifiDis(wifiTopic);
  new DbgCli_Cmd_WifiCon(wifiTopic, saveWifiCredentialsCallback);
#endif

  Serial.println();
  Serial.println("---------------------------------------------");
  Serial.println("Hello from Wiring IoT Controller!");
  Serial.println("---------------------------------------------");
  Serial.println();
}

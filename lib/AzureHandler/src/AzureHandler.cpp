/*
 * LandingPageHandler.cpp
 *
 *  Created on: 15.05.2022
 *      Author: Armando Amoros
 */

#include "AzureHandler.h"

#include <tuple>

#include <ArduinoJson.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <base64.hpp>
#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <HTTPClient.h>
#include <mbedtls/md.h>

const char s_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ
RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD
VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX
DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y
ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy
VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr
mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr
IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK
mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu
XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy
dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye
jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1
BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3
DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92
9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx
jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0
Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz
ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS
R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp
-----END CERTIFICATE-----
)EOF";
#endif

String s_mqttHostName; // this will be the Azure IoT Hub Full Name
String s_mqttUserId;   // this will be the Azure IoT Hub DeviceId
String s_mqttPassword; // this will be the Preshared Key

WiFiClientSecure s_espClient;

HTTPClient s_httpsClient;

PubSubClient s_pubSubClient(s_espClient);

WiFiUDP s_ntpUDP;
NTPClient s_timeClient(s_ntpUDP);

static String Sha256Sign(String dataToSign, String preSharedKey)
{
  unsigned char decodedPSK[32];
  unsigned char encryptedSignature[100];
  unsigned char encodedSignature[100];
  unsigned int contextSize;

  // need to base64 decode the Preshared key and the length
  int base64_decoded_device_length = decode_base64((unsigned char*) preSharedKey.c_str(), decodedPSK);

#if defined(ESP8266)
  br_sha256_context sha256_context;
  br_hmac_key_context hmac_key_context;
  br_hmac_context hmac_context;

  // create the sha256 hmac and hash the data
  br_sha256_init(&sha256_context);
  br_hmac_key_init(&hmac_key_context, sha256_context.vtable, decodedPSK, base64_decoded_device_length);
  br_hmac_init(&hmac_context, &hmac_key_context, 32);
  br_hmac_update(&hmac_context, dataToSign.c_str(), strlen(dataToSign.c_str()));
  br_hmac_out(&hmac_context, encryptedSignature);

  contextSize = br_hmac_size(&hmac_context);
#elif defined(ESP32)
  mbedtls_md_context_t ctx;

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
  mbedtls_md_hmac_starts(&ctx, decodedPSK, base64_decoded_device_length);
  mbedtls_md_hmac_update(&ctx, (const unsigned char*) dataToSign.c_str(), strlen(dataToSign.c_str()));
  mbedtls_md_hmac_finish(&ctx, encryptedSignature);
  mbedtls_md_free(&ctx);

  contextSize = mbedtls_md_get_size(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256));
#endif

  // base64 decode the HMAC to a char
  encode_base64(encryptedSignature, contextSize, encodedSignature);

  // creating the real SAS Token
  return String((char*) encodedSignature);
}

static String urlEncodeBase64(String stringToEncode)
{
  stringToEncode.replace("+", "%2B");
  stringToEncode.replace("=", "%3D");
  stringToEncode.replace("/", "%2F");
  return stringToEncode;
}

static String AzurePskToToken(const String iotHubFQDN, const String deviceId, const String preSharedKey,
                              int sasTTL = 86400)
{
  int ttl = s_timeClient.getEpochTime() + sasTTL;
  String dataToSignString = urlEncodeBase64(iotHubFQDN + "/devices/" + deviceId) + "\n" + String(ttl);
  String signedData = Sha256Sign(dataToSignString, preSharedKey);

  String realSASToken = "SharedAccessSignature sr=" + urlEncodeBase64(iotHubFQDN + "/devices/" + deviceId);
  realSASToken += "&sig=" + urlEncodeBase64(signedData) + "&se=" + String(ttl);

  return realSASToken;
}

static String AzureDpsPskToToken(const String scopeId, const String deviceId, const String preSharedKey,
                                 int sasTTL = 3600)
{
  int ttl = s_timeClient.getEpochTime() + sasTTL;
  String dataToSignString = urlEncodeBase64(scopeId + "/registrations/" + deviceId) + "\n" + String(ttl);
  String signedData = Sha256Sign(dataToSignString, preSharedKey);

  String realSASToken = "SharedAccessSignature sr=" + urlEncodeBase64(scopeId + "/registrations/" + deviceId);
  realSASToken += "&sig=" + urlEncodeBase64(signedData) + "&se=" + String(ttl) + "&skn=registration";

  return realSASToken;
}

static std::tuple<uint8_t, String> HttpsRequest(const String& url, const String& sasToken,
                                                AzureHandler::HttpsActions operation,
                                                const String& body = String())
{
  int result;

  s_httpsClient.addHeader("Content-Type", "application/json");
  s_httpsClient.addHeader("Content-Encoding", "utf-8");
  s_httpsClient.addHeader("Authorization", sasToken);
  switch (operation)
  {
    case AzureHandler::HttpsActions::POST:
      result = s_httpsClient.POST(body);
      break;
    case AzureHandler::HttpsActions::GET:
      result = s_httpsClient.GET();
      break;
    case AzureHandler::HttpsActions::PUT:
      result = s_httpsClient.PUT(body);
      break;
    default:
      break;
  }

  String responseBody = s_httpsClient.getString();
  Serial.println("HTTPS Request: " + url);
  Serial.println("HTTPS Response: " + String(result) + responseBody);

  return {result, responseBody};
}

static uint8_t DeviceRegisterStatusDps(const String& url, const String& token, const String& body,
                                       const String& key)
{
  uint8_t result = AzureHandler::DEVICE_NOT_REGISTERED;

  s_httpsClient.begin(s_espClient, url);

  auto [httpResponse, httpBody] = HttpsRequest(url, token, AzureHandler::HttpsActions::POST, body);
  if (httpResponse == HTTP_CODE_OK)
  {
    Serial.println(F("DPS: Device already registered previously"));

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, httpBody.c_str());
    JsonObject jsonObject = doc.as<JsonObject>();
    s_mqttUserId = jsonObject["deviceId"].as<String>();
    s_mqttHostName = jsonObject["assignedHub"].as<String>();
    s_mqttPassword = key;

    result = AzureHandler::SUCCESS;
  }
  else
  {
    Serial.println(F("DPS: Device NOT registered previously"));
    result = AzureHandler::DEVICE_NOT_REGISTERED;
  }

  s_httpsClient.end();

  return result;
}

static std::tuple<uint8_t, String> RegisterDeviceDps(const String& url, const String& token,
                                                     const String& body)
{
  uint8_t result = AzureHandler::DEVICE_REGISTRATION_FAILED;
  String operationId;

  s_httpsClient.begin(s_espClient, url);

  auto [httpResponse, httpBody] = HttpsRequest(url, token, AzureHandler::HttpsActions::PUT, body);
  if (httpResponse == HTTP_CODE_OK || httpResponse == HTTP_CODE_ACCEPTED)
  {
    Serial.println(F("DPS: Device registration in progress"));
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, httpBody.c_str());
    JsonObject jsonObject = doc.as<JsonObject>();
    operationId = jsonObject["operationId"].as<String>();

    result = AzureHandler::SUCCESS;
  }
  else
  {
    Serial.println(F("DPS: Device registration FAILED"));
    result = AzureHandler::DEVICE_REGISTRATION_FAILED;
  }

  s_httpsClient.end();

  return {result, operationId};
}

static uint8_t OperationStatusDps(const String& url, const String& token, const String& key)
{
  uint8_t result = AzureHandler::DEVICE_REGISTRATION_OPERATION_FAILED;

  s_httpsClient.begin(s_espClient, url);

  for (uint8_t assignedCounter = 1; assignedCounter <= AzureHandler::MAX_NUM_REGISTRATION_ATTEMPTS;
       ++assignedCounter)
  {
    auto [httpResponse, httpBody] = HttpsRequest(url, token, AzureHandler::HttpsActions::GET);
    if (httpResponse == HTTP_CODE_OK)
    {
      Serial.println(F("DPS: Device registered successfully"));

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, httpBody.c_str());
      JsonObject jsonObject = doc.as<JsonObject>()["registrationState"].as<JsonObject>();
      s_mqttUserId = jsonObject["deviceId"].as<String>();
      s_mqttHostName = jsonObject["assignedHub"].as<String>();
      s_mqttPassword = key;

      result = AzureHandler::SUCCESS;
      break;
    }
    else if (httpResponse == HTTP_CODE_ACCEPTED)
    {
      if (assignedCounter == AzureHandler::MAX_NUM_REGISTRATION_ATTEMPTS)
      {
        Serial.println(F("DPS: Device registration operation taking too long"));
        result = AzureHandler::DEVICE_REGISTRATION_OPERATION_FAILED;
        break;
      }
      else
      {
        delay(1000 * assignedCounter);
        continue;
      }
    }
    else
    {
      Serial.println(F("DPS: Device registration operation FAILED"));
      result = AzureHandler::DEVICE_REGISTRATION_OPERATION_FAILED;
      break;
    }
  }

  s_httpsClient.end();

  return result;
}

static bool ProvisionAzureDps(const String& idScope, const String& deviceId, const String& deviceKey)
{
  Serial.println(F("Starting Azure DPS registration..."));

  const String dPSSASToken = AzureDpsPskToToken(idScope, deviceId, deviceKey);

  const String dpsUrlPre = String(DPS_END_POINT) + idScope + "/registrations/" + deviceId;
  const String dpsUrlSu = "?api-version=2021-06-01";
  const String dPSPutContent = "{\"registrationId\": \"" + deviceId + "\"}";

  // espClient.setInsecure(); // For testing only
#if defined(ESP8266)
  s_espClient.setFingerprint("6E:0D:8E:1E:0F:A6:C1:15:49:2F:DC:54:AF:E1:A5:DA:5C:4D:35:F9"); // SHA1 FP DPS
#elif defined(ESP32)
  s_espClient.setCACert(s_cert);
#endif

  bool isDeviceRegistered = AzureHandler::GENERAL_ERROR;
  if (DeviceRegisterStatusDps(dpsUrlPre + dpsUrlSu, dPSSASToken, dPSPutContent, deviceKey) ==
      AzureHandler::SUCCESS)
  {
    isDeviceRegistered = AzureHandler::SUCCESS;
  }
  else
  {
    auto [status, operationId] = RegisterDeviceDps(dpsUrlPre + "/register" + dpsUrlSu, dPSSASToken,
                                                   dPSPutContent);

    if (status == AzureHandler::SUCCESS)
    {
      if (OperationStatusDps(dpsUrlPre + "/operations/" + operationId + dpsUrlSu, dPSSASToken, deviceKey) ==
          AzureHandler::SUCCESS)
      {
        isDeviceRegistered = AzureHandler::SUCCESS;
      }
    }
  }

  return isDeviceRegistered;
}

static void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  String messageTopic = topic;
  String message;
  Serial.printf("Message Arrived on Topic %s\n", topic);
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char) payload[i];
  }
  Serial.printf("            Message Body %s\n", message.c_str());

  if (messageTopic.indexOf("$iothub/methods/POST/") != -1)
  {
    // we must respond back to Direct Methods
    int rid = messageTopic.substring(messageTopic.indexOf("=") + 1).toInt();
    s_pubSubClient.publish(String("$iothub/methods/res/200/?$rid=" + String(rid)).c_str(),
                           "{\"response\":\"success\"}");
    Serial.printf("     Responsed to Method %s\n", messageTopic.c_str());
  }
}

static bool checkConnection()
{
  bool result = AzureHandler::SUCCESS;

  if (!s_pubSubClient.connected())
  {
    Serial.print(F("IoTHub: MQTT attempting connection\n"));
    String iotHubUserString = s_mqttHostName + "/" + s_mqttUserId + "/?api-version=2020-09-30";
    String iotHubPassword = AzurePskToToken(s_mqttHostName, s_mqttUserId, s_mqttPassword);

    // demonstrating PubSubClient with all the settings
    // espClient.setInsecure(); // For testing only
#if defined(ESP8266)
    s_espClient.setFingerprint("58:2C:05:CC:51:8D:66:E6:97:A9:D3:32:4E:C1:48:FE:65:68:1F:0E"); // SHA1 FP IoT
#endif
    s_pubSubClient.connect(s_mqttUserId.c_str(), iotHubUserString.c_str(), iotHubPassword.c_str(), "lwt", 1,
                           true, "offline", false);

    if (s_pubSubClient.state() == MQTT_CONNECTED)
    {
      Serial.println("IoTHub: Connection successful");
      // Cloud to device messages
      s_pubSubClient.subscribe(String("devices/" + s_mqttUserId + "/messages/devicebound/#").c_str());
      // Twin topic
      s_pubSubClient.subscribe(String("$iothub/twin/res/#").c_str());
      // Request initial twin
      s_pubSubClient.publish(String("$iothub/twin/GET/?$rid=1").c_str(), "");
      // Topic for twin updates
      s_pubSubClient.subscribe(String("$iothub/twin/PATCH/properties/desired/#").c_str());
      // Topic for direct methods
      s_pubSubClient.subscribe(String("$iothub/methods/POST/#").c_str());
    }
    else
    {
      Serial.print("IoTHub: MQTT connection failed, code = ");
      Serial.println(s_pubSubClient.state());
      result = AzureHandler::GENERAL_ERROR;
    }
  }

  return result;
}

uint8_t AzureHandler::AzureHandler::azureInit(const ConfigTypes::azureConfig& config)
{
  s_timeClient.begin();
  while (s_timeClient.getEpochTime() < 1000)
  {
    s_timeClient.update();
    delay(500);
  }

  m_isDeviceRegistered = ProvisionAzureDps(config.idScope, config.deviceID, config.deviceDerivedKey);

  if (m_isDeviceRegistered == SUCCESS)
  {
    s_pubSubClient.setServer(s_mqttHostName.c_str(), 8883);
    s_pubSubClient.setCallback(mqttCallback);
    s_pubSubClient.setBufferSize(MQTT_PACKET_SIZE);
  }

  return 0;
}

uint8_t AzureHandler::AzureHandler::azureLoop(void)
{
  if (m_isDeviceRegistered != SUCCESS)
  {
    return AZURE_HANDLER_NOT_INITIALIZED;
  }

  if (checkConnection() != SUCCESS || !s_pubSubClient.loop())
  {
    return MQTT_CLIENT_CONNECTION_FAILED;
  }

  if (!s_timeClient.update())
  {
    return NTP_CLIENT_FAILED;
  }

  return SUCCESS;
}

uint8_t AzureHandler::AzureHandler::sendTelemetry(String& message) const
{
  if (m_isDeviceRegistered != SUCCESS)
  {
    return AZURE_HANDLER_NOT_INITIALIZED;
  }

  uint8_t flag = SUCCESS;
  if (!s_pubSubClient.publish(String("devices/" + s_mqttUserId + "/messages/events/").c_str(),
                              message.c_str()))
  {
    flag = MQTT_PUBLISH_FAILED;
  }

  return flag;
}

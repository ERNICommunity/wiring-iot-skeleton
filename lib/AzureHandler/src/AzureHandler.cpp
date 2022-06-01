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
#include <base64.hpp>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <HTTPClient.h>
#include <mbedtls/md.h>

const char cert[] PROGMEM = R"EOF(
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

String mqttHostName; // this will be the Azure IoT Hub Full Name
String mqttUserId;   // this will be the Azure IoT Hub DeviceId
String mqttPassword; // this will be the Preshared Key

WiFiClientSecure espClient;

HTTPClient httpsClient;

PubSubClient pubSubClient(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

static String Sha256Sign(String dataToSign, String preSharedKey)
{
    unsigned char decodedPSK[32];
    unsigned char encryptedSignature[100];
    unsigned char encodedSignature[100];
    unsigned int contextSize;

    // need to base64 decode the Preshared key and the length
    int base64_decoded_device_length = decode_base64((unsigned char *)preSharedKey.c_str(), decodedPSK);

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
    mbedtls_md_hmac_update(&ctx, (const unsigned char *)dataToSign.c_str(), strlen(dataToSign.c_str()));
    mbedtls_md_hmac_finish(&ctx, encryptedSignature);
    mbedtls_md_free(&ctx);

    contextSize = mbedtls_md_get_size(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256));
#endif

    // base64 decode the HMAC to a char
    encode_base64(encryptedSignature, contextSize, encodedSignature);

    // creating the real SAS Token
    return String((char *)encodedSignature);
}

static String urlEncodeBase64(String stringToEncode)
{
    stringToEncode.replace("+", "%2B");
    stringToEncode.replace("=", "%3D");
    stringToEncode.replace("/", "%2F");
    return stringToEncode;
}

static String AzurePskToToken(const String iotHubFQDN, const String deviceId, const String preSharedKey, int sasTTL = 86400)
{
    int ttl = timeClient.getEpochTime() + sasTTL;
    String dataToSignString = urlEncodeBase64(iotHubFQDN + "/devices/" + deviceId) + "\n" + String(ttl);
    String signedData = Sha256Sign(dataToSignString, preSharedKey);

    String realSASToken = "SharedAccessSignature sr=" + urlEncodeBase64(iotHubFQDN + "/devices/" + deviceId);
    realSASToken += "&sig=" + urlEncodeBase64(signedData) + "&se=" + String(ttl);

    return realSASToken;
}

static String AzureDpsPskToToken(const String scopeId, const String deviceId, const String preSharedKey, int sasTTL = 3600)
{
    int ttl = timeClient.getEpochTime() + sasTTL;
    String dataToSignString = urlEncodeBase64(scopeId + "/registrations/" + deviceId) + "\n" + String(ttl);
    String signedData = Sha256Sign(dataToSignString, preSharedKey);

    String realSASToken = "SharedAccessSignature sr=" + urlEncodeBase64(scopeId + "/registrations/" + deviceId);
    realSASToken += "&sig=" + urlEncodeBase64(signedData) + "&se=" + String(ttl) + "&skn=registration";

    return realSASToken;
}

static std::tuple<uint8_t, String> HttpsRequest(const String &url, const String &sasToken, AzureHandler::HttpsActions operation, const String &body = String())
{
    int result;

    httpsClient.addHeader("Content-Type", "application/json");
    httpsClient.addHeader("Content-Encoding", "utf-8");
    httpsClient.addHeader("Authorization", sasToken);
    switch (operation)
    {
    case AzureHandler::HttpsActions::POST:
        result = httpsClient.POST(body);
        break;
    case AzureHandler::HttpsActions::GET:
        result = httpsClient.GET();
        break;
    case AzureHandler::HttpsActions::PUT:
        result = httpsClient.PUT(body);
        break;
    default:
        break;
    }

    String responseBody = httpsClient.getString();
    Serial.println("HTTPS Request: " + url);
    Serial.println("HTTPS Response: " + String(result) + responseBody);

    return {result, responseBody};
}

static uint8_t DeviceRegisterStatusDps(const String &url, const String &token, const String &body, const String &key)
{
    uint8_t result = AzureHandler::DEVICE_NOT_REGISTERED;

    httpsClient.begin(espClient, url);

    auto [httpResponse, httpBody] = HttpsRequest(url, token, AzureHandler::HttpsActions::POST, body);
    if (httpResponse == HTTP_CODE_OK)
    {
        Serial.println("DPS: Device already registered previously");

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, httpBody.c_str());
        JsonObject jsonObject = doc.as<JsonObject>();
        mqttUserId = jsonObject["deviceId"].as<String>();
        mqttHostName = jsonObject["assignedHub"].as<String>();
        mqttPassword = key;

        result = AzureHandler::SUCCESS;
    }
    else
    {
        Serial.println("DPS: Device NOT registered previously");
        result = AzureHandler::DEVICE_NOT_REGISTERED;
    }

    httpsClient.end();

    return result;
}

static std::tuple<uint8_t, String> RegisterDeviceDps(const String &url, const String &token, const String &body)
{
    uint8_t result = AzureHandler::DEVICE_REGISTRATION_FAILED;
    String operationId;

    httpsClient.begin(espClient, url);

    auto [httpResponse, httpBody] = HttpsRequest(url, token, AzureHandler::HttpsActions::PUT, body);
    if (httpResponse == HTTP_CODE_OK || httpResponse == HTTP_CODE_ACCEPTED)
    {
        Serial.println("DPS: Device registration in progress");
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, httpBody.c_str());
        JsonObject jsonObject = doc.as<JsonObject>();
        operationId = jsonObject["operationId"].as<String>();

        result = AzureHandler::SUCCESS;
    }
    else
    {
        Serial.println("DPS: Device registration FAILED");
        result = AzureHandler::DEVICE_REGISTRATION_FAILED;
    }

    httpsClient.end();

    return {result, operationId};
}

static uint8_t OperationStatusDps(const String &url, const String &token, const String &key)
{
    uint8_t result = AzureHandler::DEVICE_REGISTRATION_OPERATION_FAILED;

    httpsClient.begin(espClient, url);

    for (uint8_t assignedCounter = 1; assignedCounter <= AzureHandler::MAX_NUM_REGISTRATION_ATTEMPTS; ++assignedCounter)
    {
        auto [httpResponse, httpBody] = HttpsRequest(url, token, AzureHandler::HttpsActions::GET);
        if (httpResponse == HTTP_CODE_OK)
        {
            Serial.println("DPS: Device registered successfully");

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, httpBody.c_str());
            JsonObject jsonObject = doc.as<JsonObject>()["registrationState"].as<JsonObject>();
            mqttUserId = jsonObject["deviceId"].as<String>();
            mqttHostName = jsonObject["assignedHub"].as<String>();
            mqttPassword = key;

            result = AzureHandler::SUCCESS;
            break;
        }
        else if (httpResponse == HTTP_CODE_ACCEPTED)
        {
            if (assignedCounter == AzureHandler::MAX_NUM_REGISTRATION_ATTEMPTS)
            {
                Serial.println("DPS: Device registration operation taking too long");
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
            Serial.println("DPS: Device registration operation FAILED");
            result = AzureHandler::DEVICE_REGISTRATION_OPERATION_FAILED;
            break;
        }
    }

    httpsClient.end();

    return result;
}

static bool ProvisionAzureDps(const String &idScope, const String &deviceId, const String &deviceKey)
{
    Serial.println("Starting Azure DPS registration...");

    const String dPSSASToken = AzureDpsPskToToken(idScope, deviceId, deviceKey);

    const String dpsUrlPre = String(DPS_END_POINT) + idScope + "/registrations/" + deviceId;
    const String dpsUrlSu = "?api-version=2021-06-01";
    const String dPSPutContent = "{\"registrationId\": \"" + deviceId + "\"}";

    // espClient.setInsecure(); // For testing only
#if defined(ESP8266)
    espClient.setFingerprint("A7:48:00:FB:3B:91:1F:65:3F:B1:D9:2B:7A:DC:34:76:53:21:AF:3D"); // Finger print DPS
#elif defined(ESP32)
    espClient.setCACert(cert);
#endif

    bool isDeviceRegistered = AzureHandler::GENERAL_ERROR;
    if (DeviceRegisterStatusDps(dpsUrlPre + dpsUrlSu, dPSSASToken, dPSPutContent, deviceKey) == AzureHandler::SUCCESS)
    {
        isDeviceRegistered = AzureHandler::SUCCESS;
    }
    else
    {
        auto [status, operationId] = RegisterDeviceDps(dpsUrlPre + "/register" + dpsUrlSu, dPSSASToken, dPSPutContent);

        if (status == AzureHandler::SUCCESS)
        {
            if (OperationStatusDps(dpsUrlPre + "/operations/" + operationId + dpsUrlSu, dPSSASToken, deviceKey) == AzureHandler::SUCCESS)
            {
                isDeviceRegistered = AzureHandler::SUCCESS;
            }
        }
    }

    return isDeviceRegistered;
}

static void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    String messageTopic = topic;
    String message;
    Serial.printf("Message Arrived on Topic %s\n", topic);
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }
    Serial.printf("            Message Body %s\n", message.c_str());

    if (messageTopic.indexOf("$iothub/methods/POST/") != -1)
    {
        // we must respond back to Direct Methods
        int rid = messageTopic.substring(messageTopic.indexOf("=") + 1).toInt();
        pubSubClient.publish(String("$iothub/methods/res/200/?$rid=" + String(rid)).c_str(), "{\"response\":\"success\"}");
        Serial.printf("     Responsed to Method %s\n", messageTopic.c_str());
    }
}

static bool checkConnection()
{
    bool result = AzureHandler::SUCCESS;

    if (!pubSubClient.connected())
    {
        Serial.print("IoTHub: MQTT attempting connection\n");
        String iotHubUserString = mqttHostName + "/" + mqttUserId + "/?api-version=2020-09-30";
        String iotHubPassword = AzurePskToToken(mqttHostName, mqttUserId, mqttPassword);

        // demonstrating PubSubClient with all the settings
        // espClient.setInsecure(); // For testing only
#if defined(ESP8266)
        espClient.setFingerprint("58:2C:05:CC:51:8D:66:E6:97:A9:D3:32:4E:C1:48:FE:65:68:1F:0E"); // Finger print DMN IoT Hub
#endif
        pubSubClient.connect(mqttUserId.c_str(), iotHubUserString.c_str(), iotHubPassword.c_str(), "lwt", 1, true, "offline", false);

        if (pubSubClient.state() == MQTT_CONNECTED)
        {
            Serial.println("IoTHub: Connection successful");
            pubSubClient.subscribe(String("devices/" + mqttUserId + "/messages/devicebound/#").c_str()); // cloud to device messages
            pubSubClient.subscribe(String("$iothub/twin/res/#").c_str());                                // initial twin response
            pubSubClient.publish(String("$iothub/twin/GET/?$rid=1").c_str(), "");                        // request the initial twin
            pubSubClient.subscribe(String("$iothub/twin/PATCH/properties/desired/#").c_str());           // future twin updates
            pubSubClient.subscribe(String("$iothub/methods/POST/#").c_str());                            // direct methods
        }
        else
        {
            Serial.print("IoTHub: MQTT connection failed, code = ");
            Serial.println(pubSubClient.state());
            result = AzureHandler::GENERAL_ERROR;
        }
    }

    return result;
}

uint8_t AzureHandler::AzureHandler::azureInit(const ConfigTypes::azureConfig &config)
{
    timeClient.begin();
    while (timeClient.getEpochTime() < 1000)
    {
        timeClient.update();
        delay(500);
    }

    m_isDeviceRegistered = ProvisionAzureDps(config.idScope, config.deviceID, config.deviceDerivedKey);

    if (m_isDeviceRegistered == SUCCESS)
    {
        pubSubClient.setServer(mqttHostName.c_str(), 8883);
        pubSubClient.setCallback(mqttCallback);
        pubSubClient.setBufferSize(MQTT_PACKET_SIZE);
    }

    return 0;
}

void AzureHandler::AzureHandler::azureLoop(void)
{
    if (m_isDeviceRegistered != SUCCESS)
    {
        return;
    }

    if (checkConnection() == SUCCESS)
    {

        static long lastMsg = 0;
        // Send messages every 15 seconds
        if (millis() - lastMsg > 15000)
        {
            lastMsg = millis();

            // Do the real work here
            String json = "{\"message\": {\"epoch\": " + String(timeClient.getEpochTime()) + ", ";
            json += "\"heap\": " + String(ESP.getFreeHeap()) + "}}";
            Serial.print("Sending Message: ");
            Serial.println(json);
            Serial.print("to: ");
            Serial.println(String("devices/" + mqttUserId + "/messages/events/"));
            pubSubClient.publish(String("devices/" + mqttUserId + "/messages/events/").c_str(), json.c_str());
        }

        pubSubClient.loop();
    }

    timeClient.update();
}

#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "./blewrapper.h"

#define SERVICE_UUID "d01c3000-eb86-4576-a46d-3239440100da"
#define WIFI_STATUS_CHARACTERISTIC_UUID "d01c3001-eb86-4576-a46d-3239440100da"
#define WIFI_DEFINITION_CHARACTERISTIC_UUID "d01c3002-eb86-4576-a46d-3239440100da"
#define PUBLIC_NAME_CHARACTERISTIC_UUID "d01c3003-eb86-4576-a46d-3239440100da"

class PublicNameCharacteristiCallBack : public BLECharacteristicCallbacks
{
public:
    PublicNameCharacteristiCallBack(Esp32BleWrapper *wrapper)
    {
        m_wrapper = wrapper;
    }
    void onRead(BLECharacteristic *pCharacteristic)
    {
        String message = m_wrapper->execGetPublicName();
        pCharacteristic->setValue(message.c_str());
    }
    void onWrite(BLECharacteristic *pCharacteristic) {
        String value = pCharacteristic->getValue().c_str();
        m_wrapper->execNotifyPublicName(value);
    }

private:
    Esp32BleWrapper *m_wrapper;
};


class WifiStatusCharacteristiCallBack : public BLECharacteristicCallbacks
{
public:
    WifiStatusCharacteristiCallBack(Esp32BleWrapper *wrapper)
    {
        m_wrapper = wrapper;
    }
    void onRead(BLECharacteristic *pCharacteristic)
    {
        String message = m_wrapper->getBLEWifiStatusAsString();
        pCharacteristic->setValue(message.c_str());
    }

private:
    Esp32BleWrapper *m_wrapper;
};

class WifiCharacteristicCallback : public BLECharacteristicCallbacks
{
public:
    WifiCharacteristicCallback(Esp32BleWrapper *wrapper)
    {
        m_wrapper = wrapper;
    }
    void onRead(BLECharacteristic *pCharacteristic)
    {
        Serial.println("Read WIFIDATA");
        JsonDocument doc;
        doc["ssid"] = m_wrapper->execGetSSID();
        doc["pw"] = m_wrapper->execGetPassword();
        String message;
        serializeJson(doc, message);
        Serial.print("Payload: ");
        Serial.println(message);
        pCharacteristic->setValue(message.c_str());
    }
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        String value = pCharacteristic->getValue().c_str();
        JsonDocument doc;
        deserializeJson(doc, value);
        m_wrapper->execNotifyWifiCredentials(doc["ssid"], doc["pw"]);
    }

private:
    Esp32BleWrapper *m_wrapper;
};

Esp32BleServerCallbacks::Esp32BleServerCallbacks(Esp32BleWrapper *wrapper)
{
    m_wrapper = wrapper;
}
void Esp32BleServerCallbacks::onConnect(BLEServer *pServer)
{
    Serial.println("CONNECT");
    BLECharacteristic *wStatus = pServer->getServiceByUUID(SERVICE_UUID)->getCharacteristic(WIFI_STATUS_CHARACTERISTIC_UUID);
}

void Esp32BleServerCallbacks::onDisconnect(BLEServer *pServer)
{
    Serial.println("NOT CONNECT");
    delay(500);
    pServer->startAdvertising();
}

Esp32BleWrapper::Esp32BleWrapper()
{
    m_wifiStatus = UNKNOWN;
    m_wifiStatusCharacteristic = nullptr;
    m_wifiDefinitionCharacteristic = nullptr;
    m_publicNameCharacteristic = nullptr;
    m_updateValueCallbacks = nullptr;
}

void Esp32BleWrapper::setup(String deviceName)
{
    BLEDevice::init(deviceName.c_str());
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    createPublicNameCharacteristic(pService);
    createWifiDefinitionCharacteristic(pService);
    createWifiStatusCharacteristic(pService);
    pServer->setCallbacks(new Esp32BleServerCallbacks(this));
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
    if (this->m_wifiStatusCharacteristic == NULL)
    {
        Serial.println("SETUP:_hmm... null?");
    }
}
void Esp32BleWrapper::createWifiStatusCharacteristic(BLEService *pService)
{
    Serial.println("handling statusCharacteristic");
    m_wifiStatusCharacteristic = pService->createCharacteristic(
        WIFI_STATUS_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);
    m_wifiStatusCharacteristic->addDescriptor(new BLE2902());
    m_wifiStatusCharacteristic->setCallbacks(new WifiStatusCharacteristiCallBack(this));
}
void Esp32BleWrapper::createWifiDefinitionCharacteristic(BLEService *pService)
{
    m_wifiDefinitionCharacteristic = pService->createCharacteristic(
        WIFI_DEFINITION_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_WRITE);
    m_wifiDefinitionCharacteristic->addDescriptor(new BLE2902());
    m_wifiDefinitionCharacteristic->setCallbacks(new WifiCharacteristicCallback(this));
}
void Esp32BleWrapper::createPublicNameCharacteristic(BLEService *pService)
{
    m_publicNameCharacteristic = pService->createCharacteristic(
        PUBLIC_NAME_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);
    m_publicNameCharacteristic->addDescriptor(new BLE2902());
    m_publicNameCharacteristic->setCallbacks(new PublicNameCharacteristiCallBack(this));
}
void Esp32BleWrapper::updateWIFIStatus(BLE_WIFISTATUS wifiStatus)
{
    m_wifiStatus = wifiStatus;
    if (m_wifiStatusCharacteristic == NULL)
    {
        Serial.println("hmm... null?");
    }
    String statusAsString = getBLEWifiStatusAsString();
    m_wifiStatusCharacteristic->setValue(statusAsString.c_str());
    m_wifiStatusCharacteristic->notify(true);
}
void Esp32BleWrapper::registerUpdateValueCallBacks(Esp32UpdateValueCallbacks *callbacks)
{
    m_updateValueCallbacks = callbacks;
}
void Esp32BleWrapper::execNotifyWifiCredentials(String ssid, String password)
{
    if (m_updateValueCallbacks != nullptr)
    {
        m_updateValueCallbacks->onWifiCredentialsUpdate(ssid, password);
    }
}
void Esp32BleWrapper::execNotifyPublicName(String publicName)
{
    if (m_updateValueCallbacks != nullptr)
    {
        m_updateValueCallbacks->onPublicNameUpdate(publicName);
    }
}
String Esp32BleWrapper::execGetSSID()
{
    if (m_updateValueCallbacks != nullptr)
    {
        return m_updateValueCallbacks->getSSID();
    }
    return "";
}
String Esp32BleWrapper::execGetPassword()
{
    if (m_updateValueCallbacks != nullptr)
    {
        return m_updateValueCallbacks->getPassword();
    }
    return "";
}
String Esp32BleWrapper::execGetPublicName()
{
    if (m_updateValueCallbacks != nullptr)
    {
        return m_updateValueCallbacks->getPublicName();
    }
    return "";
}
String Esp32BleWrapper::getBLEWifiStatusAsString()
{
    switch (m_wifiStatus)
    {
    case UNKNOWN:
        return "unknown";
        break;
    case INITIALIZING:
        return "initalizing";
        break;
    case CONNECTED:
        return "connected";
        break;
    case CONNECTING:
        return "connecting";
        break;
    case DISCONNECTED:
        return "disconnected";
        break;
    case FAILED:
        return "failed";
        break;
    case NOTSUPPORTED:
        return "notsupported";
        break;
    default:
        Serial.println("strange...");
        return "strange";
    }
}

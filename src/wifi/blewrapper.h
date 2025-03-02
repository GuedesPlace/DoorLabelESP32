#ifndef BleWrapper_h
#define BleWrapper_h

#include <BLEServer.h>
#include "./datatypes.h"

class Esp32BleWrapper;

class Esp32UpdateValueCallbacks
{
public:
    virtual ~Esp32UpdateValueCallbacks(){};
    virtual void onWifiCredentialsUpdate(String ssid, String password){};
    virtual void onPublicNameUpdate(String publicName){};
    virtual String getSSID() { return ""; };
    virtual String getPassword() { return ""; };
    virtual String getPublicName() { return ""; };
};

class Esp32BleServerCallbacks : public BLEServerCallbacks
{
public:
    Esp32BleServerCallbacks(Esp32BleWrapper *wrapper);
    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);

private:
    Esp32BleWrapper *m_wrapper;
};

class Esp32BleWrapper
{
public:
    Esp32BleWrapper();
    void setup(String deviceName);
    void updateWIFIStatus(BLE_WIFISTATUS wifiStatus);
    void registerUpdateValueCallBacks(Esp32UpdateValueCallbacks *callback);
    void execNotifyWifiCredentials(String ssid, String password);
    void execNotifyPublicName(String publicName);
    String execGetSSID();
    String execGetPassword();
    String execGetPublicName();
    String getBLEWifiStatusAsString();

private:
    BLE_WIFISTATUS m_wifiStatus = UNKNOWN;
    BLECharacteristic *m_wifiStatusCharacteristic;
    BLECharacteristic *m_wifiDefinitionCharacteristic;
    BLECharacteristic *m_publicNameCharacteristic;
    Esp32UpdateValueCallbacks *m_updateValueCallbacks = nullptr;
    void createWifiStatusCharacteristic(BLEService *service);
    void createWifiDefinitionCharacteristic(BLEService *service);
    void createPublicNameCharacteristic(BLEService *service);
};

#endif

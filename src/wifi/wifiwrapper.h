#ifndef WifiWrapper_h
#define WifiWrapper_h
#include <WiFi.h>
#include "./datatypes.h"

class WifiStatusCallbacks {
    public:
        virtual ~WifiStatusCallbacks(){};
        virtual void onWifiStatusUpdate(BLE_WIFISTATUS wifiStatus);
};

class WifiWrapper {
public:
    WifiWrapper();
    void setup(String deviceName);
    void updateWifiCredentials(String ssid, String pw);
    BLE_WIFISTATUS getStatus();
    void registerWifiStatusCallbacks(WifiStatusCallbacks* callbacks);
    void checkStatus();
    bool isConnected();

private:
    void updateStatusByWifiStatus(wl_status_t newStatus);
    BLE_WIFISTATUS m_status;
    WifiStatusCallbacks* m_callbacks;
    wl_status_t m_lastStatus;
};

#endif
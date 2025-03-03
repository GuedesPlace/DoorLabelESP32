#ifndef PREFERENCE_CONTROLLER_H_
#define PREFERENCE_CONTROLLER_H_

#include <Preferences.h>

class PreferenceController {
    public:
        PreferenceController();
        void updateWifiData(String ssid, String password);
        void updateFunctionCode(String functionCode);
        void updateHashCode(String hashCode);
        void updateEndpointName(String endpointName);
        bool isWiFiConfigured();
        bool isEndpointConfigured();
        String getEndpointName();
        String getFunctionCode();
        String getSSID();
        String getPassword();
        String getHashCode();
        void loadOrUpdatePreferenceData();
    private:
        Preferences m_preferences;
        String m_ssid;
        String m_password;
        String m_hashCode;
        String m_endpointName;
        String m_functionCode;
        void updatePreferenceData();
}; 

#endif
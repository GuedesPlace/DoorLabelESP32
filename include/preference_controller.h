#ifndef PREFERENCE_CONTROLLER_H_
#define PREFERENCE_CONTROLLER_H_

#include <Preferences.h>
#include "./dl_types.h"

class PreferenceController {
    public:
        PreferenceController();
        void updateWifiData(String ssid, String password);
        void updateFunctionCode(String functionCode);
        void updateHashCode(String hashCode);
        void updateEndpointName(String endpointName);
        bool isWiFiConfigured();
        bool isEndpointConfigured();
        String getEnpointName();
        String getFunctionCode();
        String getSSID();
        String getPassword();
        String getHashCode();
        void loadOrUpdatePreferenceData();
    private:
        Preferences m_preferences;
        preferenceData m_preferenceData;
        void updatePreferenceData();
}; 

#endif
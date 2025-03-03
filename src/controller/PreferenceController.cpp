#include "preference_controller.h"

PreferenceController::PreferenceController() {
}

void PreferenceController::updateWifiData(String ssid, String password)
{
    m_preferenceData.ssid = ssid;
    m_preferenceData.password = password;
    m_preferences.begin("espDoorLabel", false);
    m_preferences.putString("ssid", ssid);
    m_preferences.putString("password", password);
    m_preferences.end();
}

void PreferenceController::updateFunctionCode(String functionCode)
{
    m_preferenceData.functionCode = functionCode;
    m_preferences.begin("espDoorLabel", false);
    m_preferences.putString("functionCode", functionCode);
    m_preferences.end();
}

void PreferenceController::updateHashCode(String hashCode)
{
    m_preferenceData.hash = hashCode;
    m_preferences.begin("espDoorLabel", false);
    m_preferences.putString("hashCode", hashCode);
    m_preferences.end();
}

void PreferenceController::updateEndpointName(String endpointName)
{
    m_preferenceData.endpointName = endpointName;
    m_preferences.begin("espDoorLabel", false);
    m_preferences.putString("endpointName", endpointName);
    m_preferences.end();
}

bool PreferenceController::isWiFiConfigured()
{
    return !m_preferenceData.ssid.isEmpty();
}

bool PreferenceController::isEndpointConfigured()
{
    return !m_preferenceData.endpointName.isEmpty() && !m_preferenceData.functionCode.isEmpty();
}
String PreferenceController::getEnpointName() {
    return m_preferenceData.endpointName;
}
String PreferenceController::getFunctionCode() {
    return m_preferenceData.functionCode;
}

String PreferenceController::getSSID()
{
    return m_preferenceData.ssid;
}

String PreferenceController::getPassword()
{
    return m_preferenceData.password;
}

String PreferenceController::getHashCode()
{
    return m_preferenceData.hash;
}

void PreferenceController::loadOrUpdatePreferenceData()
{
    updatePreferenceData();
}

void PreferenceController::updatePreferenceData()
{
    m_preferences.begin("espDoorLabel", false);
    String ssid = "";
    String password = "";
    String hash = "NOHASH";
    String functionCode = "";
    String endpointName = "";
    if (m_preferences.isKey("ssid")) {
        ssid = m_preferences.getString("ssid", "");
    }
    if (m_preferences.isKey("password")) {
        password = m_preferences.getString("password", "");
    }
    if (m_preferences.isKey("hashCode")) {
        hash = m_preferences.getString("hashCode", "NOHASH");
    }
    if (m_preferences.isKey("functionCode")) {
        functionCode = m_preferences.getString("functionCode", "");
    }
    if (m_preferences.isKey("endpointName")) {
        endpointName = m_preferences.getString("endpointName", "");
    }
    m_preferences.end();
    Serial.println("..... DONE: "+ ssid);
    m_preferenceData = {ssid, password, hash, functionCode, endpointName};
}

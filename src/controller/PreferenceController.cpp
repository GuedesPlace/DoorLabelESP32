#include "preference_controller.h"

PreferenceController::PreferenceController() {
}

void PreferenceController::updateWifiData(String ssid, String password)
{
    m_ssid = String(ssid);
    m_password = String(password);
    m_preferences.begin("espDoorLabel", false);
    m_preferences.putString("ssid", ssid);
    m_preferences.putString("password", password);
    m_preferences.end();
}

void PreferenceController::updateFunctionCode(String functionCode)
{
    m_functionCode = String(functionCode);
    m_preferences.begin("espDoorLabel", false);
    m_preferences.putString("functionCode", functionCode);
    m_preferences.end();
}

void PreferenceController::updateHashCode(String hashCode)
{
    m_hashCode = String(hashCode);
    m_preferences.begin("espDoorLabel", false);
    m_preferences.putString("hashCode", hashCode);
    m_preferences.end();
}

void PreferenceController::updateEndpointName(String endpointName)
{
    m_endpointName = String(endpointName);
    m_preferences.begin("espDoorLabel", false);
    m_preferences.putString("endpointName", endpointName);
    m_preferences.end();
}

bool PreferenceController::isWiFiConfigured()
{
    return !m_ssid.isEmpty();
}

bool PreferenceController::isEndpointConfigured()
{
    return !m_endpointName.isEmpty() && !m_functionCode.isEmpty();
}
String PreferenceController::getEndpointName() {
    return m_endpointName;
}
String PreferenceController::getFunctionCode() {
    return m_functionCode;
}

String PreferenceController::getSSID()
{
    return m_ssid;
}

String PreferenceController::getPassword()
{
    return m_password;
}

String PreferenceController::getHashCode()
{
    return m_hashCode;
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
    serial_mtx.unlock();
    m_ssid = ssid;
    m_password = password;
    m_hashCode = hash;
    m_functionCode = functionCode;
    m_endpointName = endpointName;
}

#include <GPWiFiWrapper.h>
#include <GPBleWrapper.h>
#include "preference_controller.h"

class MyEspUpdateCallbacks : public GPBleUpdateValueCallbacks
{
public:
    MyEspUpdateCallbacks(GPWifiWrapper *wrapper, PreferenceController *preferenceController)
    {
        m_wrapper = wrapper;
        m_prefController = preferenceController;
    }
    virtual void onWifiCredentialsUpdate(String ssid, String password)
    {
        m_prefController->updateWifiData(ssid, password);
        m_wrapper->updateWifiCredentials(ssid, password);
    };
    virtual void onPublicNameUpdate(String publicName) {
    };
    String getSSID()
    {
        return m_prefController->getSSID();
    };
    String getPassword()
    {
        return m_prefController->getPassword();
    };
    String getPublicName()
    {
        return "";
    };
    GPConfigurationProperties *getProperties()
    {
        GPConfigurationProperties *properties = new GPConfigurationProperties(2);
        gpConfigProperty propertyEndpoint = {"endpointname", "Name des CloudEndpints", m_prefController->getEndpointName()};
        gpConfigProperty propertyFunctionCode = {"functionCode", "API Key", m_prefController->getFunctionCode()};
        properties->setProperty(propertyEndpoint, 0);    
        properties->setProperty(propertyFunctionCode, 1);
        return properties;
    };
    void onConfigPropertyUpdate(GPConfigurationProperties *properties)
    {
        Serial.println("Update....");
        String endpoint = properties->getProperty(0).value;
        String functionCode = properties->getProperty(1).value;
        m_prefController->updateEndpointName(endpoint);
        m_prefController->updateFunctionCode(functionCode);
        
    };

private:
    GPWifiWrapper *m_wrapper;
    PreferenceController *m_prefController;
};

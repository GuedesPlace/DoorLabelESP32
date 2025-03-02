#include <GPWiFiWrapper.h>
#include <GPBleWrapper.h>


class MyWiFiStatusCallBack : public GPWifiStatusCallbacks
{
public:
    MyWiFiStatusCallBack(GPBleWrapper *wrapper)
    {
        m_wrapper = wrapper;
    }
    void onWifiStatusUpdate(GPBLE_WIFISTATUS wifiStatus)
    {
        m_wrapper->updateWIFIStatus(wifiStatus);
    }

private:
    GPBleWrapper *m_wrapper;
};

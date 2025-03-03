#ifndef PICTURE_REST_ENDPOINT_H_
#define PICTURE_REST_ENDPOINT_H_

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <pngle.h>

struct hasNewPictureResult {
    bool hasNewPicture;
    bool notConfigured;
    String hash;
};
void g_pic_init();

class PictureRestEndpoint {
    public:
        PictureRestEndpoint(String endpointName, String functionCode, String macAddress);
        ~PictureRestEndpoint();
        hasNewPictureResult hasNewPicture(float voltage, String currentHash);
        uint8_t *FetchPictureToLocalBuffer();
    private:
        String m_endpointName;
        String m_functionCode;
        String m_macAddressAsString;
        WiFiClientSecure *m_client;
};

#endif
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

class PictureRestEndpoint {
    public:
        PictureRestEndpoint(String endpointName, String functionCode, String macAddress);
        ~PictureRestEndpoint();
        hasNewPictureResult hasNewPicture(float voltage, String currentHash);
        uint8_t *FetchPictureToLocalBuffer();
        void pngle_on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]);
    private:
        
        String m_endpointName;
        String m_functionCode;
        String m_macAddressAsString;
        uint8_t *m_received;
        WiFiClientSecure *m_client;
};

#endif
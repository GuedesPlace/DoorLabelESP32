#include "dl_rest.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <pngle.h>

#include "./dl_constants.h"

uint8_t *g_received;

void pngle_on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
    // uint16_t color = (rgba[0] << 8 & 0xf800) | (rgba[1] << 3 & 0x07e0) | (rgba[2] >> 3 & 0x001f);
    uint8_t r = rgba[0]; // 0 - 255
    uint8_t g = rgba[1]; // 0 - 255
    uint8_t b = rgba[2]; // 0 - 255
    uint8_t a = rgba[3]; // 0: fully transparent, 255: fully opaque

    // if (a) printf("put pixel at (%d, %d) with color #%02x%02x%02x\n", x, y, r, g, b);
    int position = (x - 1) + ((y - 1) * 540);
    if (position > 540 * 960)
    {
        Serial.print("ERROR... X: ");
        Serial.print(x);
        Serial.print(" Y:");
        Serial.print(y);
        Serial.print(" Position:");
        Serial.println(position);
    }
    else
    {
        g_received[position] = (uint8_t)(r * 0.2126) + (g * 0.7152) + (b * 0.0722);
    }
}

void dl_rest_init()
{
    g_received = (uint8_t *)heap_caps_malloc(screen_width * screen_height, MALLOC_CAP_SPIRAM);
    memset(g_received, 0xFF, screen_width * screen_height);
}
hasNewPictureResult dl_rest_hasNewPicture(float voltage, String currentHash)
{
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        // set secure client without certificate
        client->setInsecure();
        // create an HTTPClient instance
        HTTPClient https;

        // Initializing an HTTPS communication using the secure client
        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, "https://gpdoorlabelep.azurewebsites.net/api/device/99_88_77_66/"+currentHash+"?code=wOkBLLCM7-mQJn736jhLuNciDOQYk3SSFR-qt9xtNNDiAzFuxQA74Q%3D%3D"))
        {
            https.addHeader("Content-Type", "application/json");
            String toPost = "{\"version\":\"1.0.0\",\"batteryVoltage\":\"" + String(voltage) + "\"}";
            int httpCode = https.POST(toPost);
            if (httpCode > 0)
            {
                String input = https.getString();

                JsonDocument doc;

                DeserializationError error = deserializeJson(doc, input);

                if (error)
                {
                    Serial.print("deserializeJson() failed: ");
                    Serial.println(error.c_str());
                    return {false, ""};
                }

                const char *status = doc["status"]; // "changed"
                const char *hash = doc["hash"];     // "51536a0ad51747c9ef52c01675ce20ee886c683ec069a86a42198aca1dbfe525"
                String thisStatus = String(status);
                Serial.println("STATUS: "+ thisStatus);
                https.end();
                if (thisStatus.equals("changed")) {

                }
                return {thisStatus.equals("changed"), String(hash)};
            }
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            https.end();
        }
    }
    return {false, ""};
}
uint8_t *dl_rest_FetchPicture()
{
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        // set secure client without certificate
        client->setInsecure();
        // create an HTTPClient instance
        HTTPClient https;

        // Initializing an HTTPS communication using the secure client
        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, "https://gpdoorlabelep.azurewebsites.net/api/deviceimage/99_88_77_66?code=_W6AXopCE-CsLcBg2FP_VyPc3nwcnuqVJ09_9r9SpQX5AzFuzz1-nQ%3D%3D"))
        { // HTTPS
            Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTP header
            int httpCode = https.GET();
            // httpCode will be negative on error
            if (httpCode > 0)
            {
                WiFiClient *stream = https.getStreamPtr();
                pngle_t *pngle = pngle_new();
                pngle_set_draw_callback(pngle, pngle_on_draw);

                uint8_t buf[2048];
                int remain = 0;
                int retry = 0;
                while (https.connected())
                {
                    size_t size = stream->available();
                    if (!size)
                    {
                        if (retry < 2)
                        {
                            Serial.print("Ready for retry: ");
                            Serial.println(retry);
                            delay(10);
                            retry++;
                            continue;
                        }
                        Serial.println("Cuttoff");
                        break;
                    }

                    if (size > sizeof(buf) - remain)
                    {
                        size = sizeof(buf) - remain;
                    }

                    int len = stream->readBytes(buf + remain, size);
                    if (len > 0)
                    {
                        int fed = pngle_feed(pngle, buf, remain + len);
                        if (fed < 0)
                        {
                            Serial.println("ERRORO");
                            break;
                        }

                        remain = remain + len - fed;
                        if (remain > 0)
                        {
                            memmove(buf, buf + fed, remain);
                        }
                    }
                    else
                    {
                        delay(1);
                    }
                }

                pngle_destroy(pngle);
                Serial.println("DATA RECEIVED");
            }
            else
            {
                Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                https.end();
                return nullptr;
            }
            https.end();
            return g_received;
        }
    }
    return nullptr;
}
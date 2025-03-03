#include "picture_rest_endpoint.h"
#include "ArduinoJson.h"
#include <HTTPClient.h>
#include <pngle.h>

const uint32_t screen_width = 960;
const uint32_t screen_height = 540;

PictureRestEndpoint *pictureRestEndpointPtr;
void onDraw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
    return pictureRestEndpointPtr->pngle_on_draw(pngle, x, y, w, h, rgba);
}

PictureRestEndpoint::PictureRestEndpoint(String endpointName, String functionCode, String macAddress)
{
    m_endpointName = endpointName;
    m_functionCode = functionCode;
    m_macAddressAsString = macAddress;
    m_macAddressAsString.replace(":", "_");
    m_received = (uint8_t *)heap_caps_malloc(screen_width * screen_height, MALLOC_CAP_SPIRAM);
    memset(m_received, 0xFF, screen_width * screen_height);
    m_client = new WiFiClientSecure();
}

hasNewPictureResult PictureRestEndpoint::hasNewPicture(float voltage, String currentHash)
{
    if (m_client)
    {
        // set secure client without certificate
        m_client->setInsecure();
        // create an HTTPClient instance
        HTTPClient https;
        // Initializing an HTTPS communication using the secure client
        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*m_client, "https://" + m_endpointName + ".azurewebsites.net/api/device/" + m_macAddressAsString + "/" + currentHash + "?code=" + m_functionCode))
        {
            https.addHeader("Content-Type", "application/json");
            String toPost = "{\"version\":\"1.0.0\",\"batteryVoltage\":\"" + String(voltage) + "\"}";
            int httpCode = https.POST(toPost);
            Serial.print("RESPONSE CODE: ");
            Serial.println(httpCode);
            if (httpCode == 204)
            {
                hasNewPictureResult result;
                result.hash = "";
                result.hasNewPicture = false;
                result.notConfigured = true;
                https.end();
                return result;
            }
            if (httpCode > 0)
            {
                String input = https.getString();
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, input);
                if (error)
                {
                    Serial.print("deserializeJson() failed: ");
                    Serial.println(error.c_str());
                    hasNewPictureResult result;
                    result.hash = "";
                    result.hasNewPicture = false;
                    result.notConfigured = false;
                    return result;
                }

                const char *status = doc["status"]; // "changed"
                const char *hash = doc["hash"];     // "51536a0ad51747c9ef52c01675ce20ee886c683ec069a86a42198aca1dbfe525"
                String thisStatus = String(status);
                https.end();
                hasNewPictureResult result;
                result.hash = String(hash);
                result.hasNewPicture = thisStatus.equals("changed");
                result.notConfigured = false;
                return result;
            }
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            https.end();
        }
    }
    hasNewPictureResult result;
    result.hash = "";
    result.hasNewPicture = false;
    result.notConfigured = false;
    return result;
}
uint8_t *PictureRestEndpoint::FetchPictureToLocalBuffer()
{
    if (m_client)
    {
        // set secure client without certificate
        m_client->setInsecure();
        // create an HTTPClient instance
        HTTPClient https;

        // Initializing an HTTPS communication using the secure client
        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*m_client, "https://" + m_endpointName + ".azurewebsites.net/api/deviceimage/" + m_macAddressAsString + "?code=" + m_functionCode))
        { // HTTPS
            Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTP header
            int httpCode = https.GET();
            // httpCode will be negative on error
            if (httpCode > 0)
            {
                WiFiClient *stream = https.getStreamPtr();
                pngle_t *pngle = pngle_new();
                pictureRestEndpointPtr = this;
                // pngle_draw_callback_t onDrawCB = std::bind(&PictureRestEndpoint::pngle_on_draw, this);
                // pngle_set_draw_callback(pngle, onDrawCB);
                // pngle_set_draw_callback(pngle, callback);
                pngle_set_draw_callback(pngle, &onDraw);
                uint8_t buf[2048];
                int remain = 0;
                int retry = 0;
                while (https.connected())
                {
                    size_t size = stream->available();
                    
                    if (!size)
                    {
                        if (retry < 10)
                        {
                            Serial.print("Ready for retry: ");
                            Serial.println(retry);
                            delay(100);
                            retry++;
                            continue;
                        }
                        Serial.println("Cuttoff");
                        break;
                    } else {
                        retry = 0;
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
            return m_received;
        }
    }
    return nullptr;
}
PictureRestEndpoint::~PictureRestEndpoint()
{
    heap_caps_free(m_received);
}

void PictureRestEndpoint::pngle_on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
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
        m_received[position] = (uint8_t)(r * 0.2126) + (g * 0.7152) + (b * 0.0722);
    }
}
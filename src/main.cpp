#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "epd_driver.h"
#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <GPBleWrapper.h>
#include <GPWiFiWrapper.h>

#include "esp_adc_cal.h"

#include <pngle.h>
#include "./main.h"
#include "preference_controller.h"
#include "dl_types.h"
#include "./controller/MyWifiStatusCallBack.h"
#include "./controller/MyEspUpdateControllerCallback.h"
#include "picture_rest_endpoint.h"
#include "display_controller.h"
#include "text_display_controller.h"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

bool g_flip180 = true;
int vref = 1100;
bool g_useUSB = false;
GPBleWrapper bleWrapper;
GPWifiWrapper wifiWrapper;
PreferenceController preferenceController;

String GetHostName()
{
    char buffer[32];
    sprintf(buffer, "%012llx", ESP.getEfuseMac());
    return String(buffer);
}

static void print_wakeup_reason()
{
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("Wakeup caused by external signal using RTC_IO");
        break;

    case ESP_SLEEP_WAKEUP_EXT1:
        Serial.println("Wakeup caused by external signal using RTC_CNTL");
        break;

    case ESP_SLEEP_WAKEUP_TIMER:
        Serial.println("Wakeup caused by timer");
        break;

    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        Serial.println("Wakeup caused by touchpad");
        break;

    case ESP_SLEEP_WAKEUP_ULP:
        Serial.println("Wakeup caused by ULP program");
        break;

    default:
        Serial.printf(
            "Wakeup was not caused by deep sleep: %d\r\n",
            wakeup_reason);
        break;
    }
}
void goToDeepSleep() {
    Serial.println("DONE");
    WiFi.disconnect();
    epd_poweroff_all();

    Serial.println("Go to deep sleep");
    esp_sleep_enable_timer_wakeup((uint64_t)360 * (uint64_t)60000000);
    esp_deep_sleep_start();
    Serial.println("Go to deep sleep");
}

void WriteNoWifi() {
    if (!g_useUSB) {
        goToDeepSleep();
        return;
    }
    TextDisplayController *tdc = new TextDisplayController();
    tdc->updateDisplayNoWifi();
    delete tdc;
    delay(60000);
}
void WriteNoEndpoint() {
    if (!g_useUSB) {
        goToDeepSleep();
        return;
    }
    TextDisplayController *tdc = new TextDisplayController();
    tdc->updateDisplayNotConfigured();
    delete tdc;
    delay(60000);
}
void WriteNotRegistered() {
    if (!g_useUSB) {
        goToDeepSleep();
        return;
    }
    TextDisplayController *tdc = new TextDisplayController();
    tdc->updateDisplayNotRegistred(preferenceController.getEnpointName(),  wifiWrapper.getMacAddress());
    delete tdc;
    delay(60000);
}

void loop()
{
    uint16_t v = analogRead(BATT_PIN);
    float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    String voltage = "➸ Voltage: " + String(battery_voltage) + "V";
    Serial.println(voltage);
    g_useUSB = battery_voltage > 4.3;
    preferenceController.loadOrUpdatePreferenceData();
    Serial.println(".. WIFI: "+preferenceController.getSSID());
    if (!preferenceController.isWiFiConfigured()) {
        WriteNoWifi();
        return;
    }
    if (!preferenceController.isEndpointConfigured()) {
        WriteNoEndpoint();
        return;
    }
    wl_status_t currentStatus = WiFi.status();
    if (currentStatus == WL_CONNECTED)
    {
        Serial.println("WIFICONNECTED");
        PictureRestEndpoint *pre = new PictureRestEndpoint(preferenceController.getEnpointName(),preferenceController.getFunctionCode(),wifiWrapper.getMacAddress());
        hasNewPictureResult result = pre->hasNewPicture(battery_voltage, preferenceController.getHashCode());
        if (result.notConfigured) {
            WriteNotRegistered();
            return;
        }
        if (result.hasNewPicture)
        {
            Serial.println(result.hash);
            preferenceController.updateHashCode(result.hash);
            delay(5000);
            uint8_t *picReceived = pre->FetchPictureToLocalBuffer();
            if (picReceived != nullptr)
            {
                DisplayController *displayController = new DisplayController(g_flip180);
                displayController->updateDisplayWithPicture(picReceived);
                delete displayController;
            }
            else
            {
                Serial.println("Nullpointer????");
            }
        }
    }
    else
    {
        Serial.println("--- NO WIFI ---");
    }
    if (g_useUSB)
    {
        delay(60000);
    }
    else
    {
        goToDeepSleep();
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("STARTING...");
    esp_adc_cal_characteristics_t adc_chars;
#if defined(T5_47)
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
        ADC_UNIT_1,
        ADC_ATTEN_DB_12,
        ADC_WIDTH_BIT_12,
        1100,
        &adc_chars);
    g_flip180 = true;
#else
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
        ADC_UNIT_2,
        ADC_ATTEN_DB_12,
        ADC_WIDTH_BIT_12,
        1100,
        &adc_chars);
    g_flip180 = false;
#endif
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        Serial.printf("eFuse Vref: %umV\r\n", adc_chars.vref);
        vref = adc_chars.vref;
    }

    print_wakeup_reason();
    epd_poweron();
    delay(10);
    epd_init();
    Serial.println(GetHostName());
    uint16_t v = analogRead(BATT_PIN);
    float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    g_useUSB = battery_voltage > 4.3;
    preferenceController.loadOrUpdatePreferenceData();
    if (g_useUSB)
    {
        String deviceName = GetHostName();
        Serial.print("Device: ");
        Serial.println(deviceName);
        bleWrapper.setup(deviceName);
        bleWrapper.registerUpdateValueCallBacks(new MyEspUpdateCallbacks(&wifiWrapper, &preferenceController));
        wifiWrapper.setup(deviceName);
        wifiWrapper.registerWifiStatusCallbacks(new MyWiFiStatusCallBack(&bleWrapper));
        if (!preferenceController.getSSID().isEmpty())
        {
            wifiWrapper.updateWifiCredentials(preferenceController.getSSID(), preferenceController.getPassword());
            delay(1000);
            wifiWrapper.checkStatus();
        }
        delay(2000);
    }
    else
    {
        WiFi.mode(WIFI_STA);
        if (!preferenceController.getSSID().isEmpty())
        {
            WiFi.begin(preferenceController.getSSID(), preferenceController.getPassword());
        }
        delay(2000);
    }
}

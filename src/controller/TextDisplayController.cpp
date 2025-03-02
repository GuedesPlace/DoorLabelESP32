#include <Arduino.h>
#include "epd_driver.h"
#include "text_display_controller.h"
#include "firasans.h"

TextDisplayController::TextDisplayController()
{
    epd_clear();
}
void TextDisplayController::updateDisplayNoWifi() {
    writeHeader();
    int cursor_x = 200;
    int cursor_y = 250;
    const char *header = "- Kein WIFI Konfiguriert";
    writeln((GFXfont *)&FiraSans, header, &cursor_x, &cursor_y, NULL);
    delay(10);
}
void TextDisplayController::updateDisplayNotConfigured() {
    writeHeader();
    int cursor_x = 200;
    int cursor_y = 250;
    const char *header = "- Kein Endpoint konfiguriert";
    writeln((GFXfont *)&FiraSans, header, &cursor_x, &cursor_y, NULL);
    delay(10);

}
void TextDisplayController::updateDisplayNotRegistred(String endpointName, String macAddress) {
    String macAddressAsString = macAddress;
    macAddressAsString.replace(":", "_");
    Serial.println(macAddress);
    Serial.println(endpointName);
    writeHeader();
    int cursor_x = 200;
    int cursor_y = 250;
    const char *message = "- Das Display ist nicht registriert.";
    String macString = "- MAC: "+macAddressAsString;
    String endpointString = "- Endpoint: "+endpointName;
    writeln((GFXfont *)&FiraSans, message, &cursor_x, &cursor_y, NULL);
    delay(10);
    cursor_x = 200;
    cursor_y += 50;
    writeln((GFXfont *)&FiraSans, macString.c_str(), &cursor_x, &cursor_y, NULL);
    delay(10);
    cursor_x = 200;
    cursor_y += 50;
    writeln((GFXfont *)&FiraSans, endpointString.c_str(), &cursor_x, &cursor_y, NULL);
    delay(10);
}
TextDisplayController::~TextDisplayController()
{
}
void TextDisplayController::writeHeader()
{
    int cursor_x = 200;
    int cursor_y = 150;
    const char *header = "GuedesPlace DoorLabel Display\n";
    writeln((GFXfont *)&FiraSans, header, &cursor_x, &cursor_y, NULL);
    delay(50);
}
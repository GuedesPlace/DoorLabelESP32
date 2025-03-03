#include "display_controller.h"
#include "epd_driver.h"
#include <Arduino.h>

DisplayController::DisplayController(bool flip180)
{
    m_framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    memset(m_framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    m_flip180 = flip180;
}
void DisplayController::updateDisplayWithPicture(uint8_t *received)
{
    Rect_t area = {
        .x = 0,
        .y = 0,
        .width = EPD_WIDTH,
        .height = EPD_HEIGHT};

    uint8_t *translated; //[259200];
    translated = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    int counter = 0;
    if (m_flip180)
    {
        for (int x = 0; x < EPD_HEIGHT; x++)
        {
            for (int y = EPD_WIDTH - 1; y >= 0; y--)
            {
                if (y % 2 == 0)
                {
                    translated[counter] = received[x + (y * 540)];
                    counter++;
                }
            }
        }
    }
    else
    {
        Serial.println("NO FLIP");
        for (int x = EPD_HEIGHT -1; x >=0; x--)
        {
            for (int y = 0; y < EPD_WIDTH; y++)
            {
                if (y % 2 == 0)
                {
                    translated[counter] = received[x + (y * 540)];
                    counter++;
                }
            }
        }

    }

    epd_copy_to_framebuffer(area, translated, m_framebuffer);
    intUpdateDisplay(1000);
    Serial.println("--->Power down / Clean Heap");
    heap_caps_free(translated);
}

void DisplayController::intUpdateDisplay(uint32_t delay_ms)
{
    epd_clear();
    delay(delay_ms);
    volatile uint32_t t1 = millis();
    epd_draw_grayscale_image(epd_full_screen(), m_framebuffer);
    volatile uint32_t t2 = millis();
    Serial.printf("EPD draw took %dms.\r\n", t2 - t1);
}

DisplayController::~DisplayController()
{
    heap_caps_free(m_framebuffer);
};
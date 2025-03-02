#include <Arduino.h>
#include "./dl_display.h"
#include "epd_driver.h"
#include "./dl_constants.h"

uint8_t *framebuffer;
void dl_init()
{
    epd_init();

    framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
}
void dl_updateDisplay(uint32_t delay_ms)
{
    epd_clear();
    delay(10);
    volatile uint32_t t1 = millis();
    epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    volatile uint32_t t2 = millis();
    Serial.printf("EPD draw took %dms.\r\n", t2 - t1);
    delay(delay_ms);
}
void dl_translate(uint8_t *received)
{
    Rect_t area = {
        .x = 0,
        .y = 0,
        .width = screen_width,
        .height = screen_height};

    uint8_t *translated; //[259200];
    translated = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    int counter = 0;
    for (int x = 0; x < 540; x++)
    {
        for (int y = 959; y >= 0; y--)
        {
            if (y % 2 == 0)
            {
                translated[counter] = received[x + (y * 540)];
                counter++;
            }
        }
    }

    epd_copy_to_framebuffer(area, translated, framebuffer);
    dl_updateDisplay(3000);
    Serial.println("--->Power down / Clean Heap");
    heap_caps_free(translated);
}
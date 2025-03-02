#include <cstdint>
#ifndef DLDisplay_h
#define DLDisplay_h

void dl_init();
void dl_updateDisplay(uint32_t delay_ms);
void dl_translate(uint8_t *received);
#endif
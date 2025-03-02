#ifndef MAIN_H_
#define MAIN_H_

#if defined(CONFIG_IDF_TARGET_ESP32)
#define T5_47
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define T5_47_PLUS
#else
#error "Unsupported board"
#endif

#if defined(T5_47)
#define BATT_PIN (36)
#elif defined(T5_47_PLUS)
#define BATT_PIN (14)
#endif

#endif
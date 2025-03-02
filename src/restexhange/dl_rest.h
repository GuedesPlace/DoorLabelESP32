#include <cstdint>
#include <Arduino.h>
#ifndef DLRest_h
#define DLRest_h

struct hasNewPictureResult {
    bool hasNewPicture;
    String hash;
};


void dl_rest_init();
hasNewPictureResult dl_rest_hasNewPicture(float voltage, String currentHash);
uint8_t *dl_rest_FetchPicture();

#endif
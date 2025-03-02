#include <Arduino.h>
#ifndef PREFERENCE_DATA_H_
#define PREFERENCE_DATA_H_
struct preferenceData{
   String ssid;
   String password;
   String hash;
   String functionCode;
   String endpointName;
};

#endif
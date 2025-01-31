#ifndef TIMEFUNCTIONS_H
#define TIMEFUNCTIONS_H

#include "mbed.h"

// Define the timeStruc structure
struct timeStruc {
  int year;
  byte month;
  byte day;
  byte hour;
  byte minute;
  byte second;
  bool validTime;
};

// Function declarations
timeStruc SL3Time();
timeStruc GPSTime();
timeStruc mComTime();
String sendSL3Get(String input);
String mComGet(String input);
#endif // TIMEFUNCTIONS_H

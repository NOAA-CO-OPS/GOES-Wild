#ifndef TIME_LEFT_H
#define TIME_LEFT_H

#include <Arduino.h>  // Include if you're using Arduino
#include "TimeFunctions.h"

signed long totalSeconds(String input);
signed long timeLeft(String first, String period, timeStruc hrMinSec);
signed int numberOfChar(String window, String baudRate);


#endif

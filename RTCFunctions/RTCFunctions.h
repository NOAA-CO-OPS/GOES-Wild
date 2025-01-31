#ifndef RTCFUNCTIONS_H
#define RTCFUNCTIONS_H

#include <mbed.h>
#include <mbed_mktime.h>
#include "TimeFunctions.h"

bool RTCSet(int source) {
  timeStruc timeData;

  switch (source) {
    case 1:
      timeData = SL3Time();
      break;
    case 2:
      timeData = mComTime();
      break;
    case 9:
      timeData = GPSTime();
      break;
    default:
      Serial.println("Invalid source");
      return false;
  }

  if (timeData.validTime) {
    tm t;
    t.tm_sec = timeData.second;                  // 0-59
    t.tm_min = timeData.minute;                  // 0-59
    t.tm_hour = timeData.hour;                   // 0-23
    t.tm_mday = timeData.day;                    // 1-31
    t.tm_mon = timeData.month - 1;               // 0-11, "0" = Jan
    t.tm_year = ((timeData.year - 2000) + 100);  // year since 1900
    set_time(mktime(&t));                        // set RTC clock
    Serial.println("RTC set successfully");
    return true;
  } else {
    Serial.println("Failed to set RTC: Invalid time data");
    return false;
  }
}


String RTCTimeString() {
  char buffer[32];
  tm t;
  _rtc_localtime(time(NULL), &t, RTC_4_YEAR_LEAP_YEAR_SUPPORT);
  strftime(buffer, 32, "%Y-%m-%d %k:%M:%S", &t);
  return String(buffer);
}

timeStruc RTCTimeStruc() {
  timeStruc rtcTime = { 0, 0, 0, 0, 0, 0, false };
  tm t;
  _rtc_localtime(time(NULL), &t, RTC_4_YEAR_LEAP_YEAR_SUPPORT);

  rtcTime.year = t.tm_year + 1900;  // Convert from years since 1900
  rtcTime.month = t.tm_mon + 1;     // Convert month from 0-11 to 1-12
  rtcTime.day = t.tm_mday;
  rtcTime.hour = t.tm_hour;
  rtcTime.minute = t.tm_min;
  rtcTime.second = t.tm_sec;
  rtcTime.validTime = true;

  return rtcTime;
}

String RTCTimeFile() {
  char buffer[32];
  tm t;
  _rtc_localtime(time(NULL), &t, RTC_4_YEAR_LEAP_YEAR_SUPPORT);
  strftime(buffer, 32, "%y%m%d", &t);
  return String(buffer);
}

#endif  // RTCFUNCTIONS_H

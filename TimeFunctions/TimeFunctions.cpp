#include "TimeFunctions.h"
#include <TinyGPS++.h>

// TinyGPSPlus for GPS handling
TinyGPSPlus gps;

// Function to get the current transmitter's date and time as strings
timeStruc SL3Time() {
  timeStruc timeData = { 0, 0, 0, 0, 0, 0, false };
  String tempTime = sendSL3Get("!TIME");

  // Check if the string length is appropriate
  if (tempTime.length() == 19) {  // Ensure length matches "YYYY/MM/DD hh:mm:ss"
    timeData.year = tempTime.substring(0, 4).toInt();
    timeData.month = tempTime.substring(5, 7).toInt();
    timeData.day = tempTime.substring(8, 10).toInt();
    timeData.hour = tempTime.substring(11, 13).toInt();
    timeData.minute = tempTime.substring(14, 16).toInt();
    timeData.second = tempTime.substring(17, 19).toInt();
    timeData.validTime = true;  // Assuming the time is valid if parsing is successful
  } else {
    timeData.validTime = false;  // Set to false if the format is not as expected
  }

  return timeData;
}


// Function to get the current transmitter's date and time as strings
timeStruc mComTime() {
  timeStruc timeData = { 0, 0, 0, 0, 0, 0, false };
  String tempDat = mComGet("DAT?");
  String tempTim = mComGet("TIM?");
  String tempTime = tempDat + " " + tempTim;


  // Check if the string length is appropriate
  if (tempTime.length() == 19) {  // Ensure length matches "YYYY/MM/DD hh:mm:ss"
    timeData.year = tempTime.substring(6, 10).toInt();
    timeData.month = tempTime.substring(0, 2).toInt();
    timeData.day = tempTime.substring(3, 5).toInt();
    timeData.hour = tempTime.substring(11, 13).toInt();
    timeData.minute = tempTime.substring(14, 16).toInt();
    timeData.second = tempTime.substring(17, 19).toInt();
    timeData.validTime = true;  // Assuming the time is valid if parsing is successful
  } else {
    timeData.validTime = false;  // Set to false if the format is not as expected
  }

  return timeData;
}



// Function to get GPS time data
timeStruc GPSTime() {
  timeStruc timeData = { 0, 0, 0, 0, 0, 0, false };

  unsigned long startTime = millis();
  bool lastCheck = false;
  int lastSecond;

  while (millis() - startTime < 3000) {  // Timeout after 3 seconds
    while (Serial2.available() > 0) {
      gps.encode(Serial2.read());
    }

    if (gps.location.isValid() && gps.date.isUpdated() && gps.time.isUpdated()) {
      timeData.year = gps.date.year();
      timeData.month = gps.date.month();
      timeData.day = gps.date.day();
      timeData.hour = gps.time.hour();
      timeData.minute = gps.time.minute();
      timeData.second = gps.time.second();
      if (!lastCheck) {
        lastCheck = true;
        lastSecond = timeData.second;  // Update lastSecond with current second value
      }
      if (timeData.second != lastSecond) {
        timeData.validTime = true;
        return timeData;  // Return the valid time once the second has changed
      }

      lastSecond = timeData.second;  // Update lastSecond with current second value
    }
  }

  // Return invalid time if timeout occurs
  return timeData;
}

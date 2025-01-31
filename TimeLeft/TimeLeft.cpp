#include "TimeLeft.h"

signed long timeLeft(String first, String period, timeStruc hrMinSec) {
  signed long firstTotalSeconds = totalSeconds(first);
  signed long periodTotalSeconds = totalSeconds(period);



  signed long nowSeconds = hrMinSec.hour * 3600 + hrMinSec.minute * 60 + hrMinSec.second;
  firstTotalSeconds = (firstTotalSeconds - 180) % periodTotalSeconds;

  signed long timeLeft = (periodTotalSeconds - ((nowSeconds % periodTotalSeconds) - firstTotalSeconds) % periodTotalSeconds) % periodTotalSeconds;

  return timeLeft;
}

signed long totalSeconds(String input) {
  int Hours = input.substring(0, 2).toInt();
  int Minutes = input.substring(3, 5).toInt();
  int Seconds = input.substring(6, 8).toInt();
  return Hours * 3600 + Minutes * 60 + Seconds;
}

signed int numberOfChar(String window, String baudRate) {
  int windowInt = totalSeconds(window);
  int baudInt = baudRate.toInt();
  float output = (((windowInt * baudInt) / 8) * 0.872) - 76;
  return (int)round(output);
}
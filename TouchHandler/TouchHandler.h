#ifndef TOUCHHANDLER_H
#define TOUCHHANDLER_H

// Structure to hold the x and y touch values
struct TouchPoint {
  int x;
  int y;
};

// Initializes the touch controller
bool initTouch();

// Retrieves the last touch point
TouchPoint getTouchPoint();

#endif

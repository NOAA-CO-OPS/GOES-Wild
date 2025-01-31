#include "TouchHandler.h"
#include "Arduino_GigaDisplayTouch.h"

// Global touch detector object
Arduino_GigaDisplayTouch touchDetector;

// Global variable to store the last detected touch point
TouchPoint lastTouch = {-1, -1};

// This function is called whenever a touch is detected
void gigaTouchHandler(uint8_t contacts, GDTpoint_t* points) {
  if (contacts > 0) {
    lastTouch.x = points[0].x;
    lastTouch.y = points[0].y;
  }
}

// Initialize the touch detector and set up the touch handler
bool initTouch() {
  if (touchDetector.begin()) {
    touchDetector.onDetect(gigaTouchHandler);
    return true;
  }
  return false;
}

// Returns the last detected touch point
TouchPoint getTouchPoint() {
  TouchPoint currentTouch = lastTouch;

  // Reset after reading to avoid repeated detection
  lastTouch.x = -1;
  lastTouch.y = -1;

  return currentTouch;
}

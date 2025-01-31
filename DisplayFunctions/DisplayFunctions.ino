#include "DisplayFunctions.h"

// Define the display and backlight objects here
GigaDisplay_GFX display;
GigaDisplayBacklight backlight;

void initDisplay() {
  display.begin();  // Initialize the display
}

unsigned long testRoundRects() {
  int w, i, i2,
      cx = display.width() / 2 - 1,
      cy = display.height() / 2 - 1;

  display.fillScreen(BLACK);
  w = min(display.width(), display.height());

  int totalIterations = w / 6; // Number of iterations in the loop
  int delayPerIteration = 000 / totalIterations; // Delay to make the loop last 5 seconds

  for (i = 0; i < w; i += 6) {
    i2 = i / 2;
    display.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, display.color565(i, 0, 0));

    // Add delay to slow down the drawing process
    delay(delayPerIteration);
  }
  displayText("MOTUS WILDLIFE\n TRACKING SYSTEM\nGOES TRANSMITTER\nINTERFACE BOARD", BLACK, RED, 5);
}


unsigned long testFilledRoundRects() {
  int i, i2,
      cx = display.width() / 2 - 1,
      cy = display.height() / 2 - 1;

  display.fillScreen(BLACK);
  for (i = min(display.width(), display.height()); i > 20; i -= 6) {
    i2 = i / 2;
    display.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, display.color565(0, i, 0));
    yield();
  }
}

void displayText(const char* text, uint16_t fillColor, uint16_t textColor, uint8_t textSize) {
  display.fillScreen(fillColor);   // Set the fill color
  display.setRotation(1);          // Set rotation (optional)
  display.setCursor(150, 150);     // Position the cursor
  display.setTextSize(textSize);   // Set text size
  display.setTextColor(textColor); // Set text color
  display.print(text);             // Print the provided text
}

#ifndef DISPLAYFUNCTIONS_H
#define DISPLAYFUNCTIONS_H

#include "Arduino_GigaDisplay_GFX.h"
#include <Arduino_GigaDisplay.h>

// Declare the display and backlight objects globally
extern GigaDisplay_GFX display;  // 'extern' to indicate that these variables are defined elsewhere
extern GigaDisplayBacklight backlight;

// Define color constants
#define CYAN 0x07FF
#define RED 0xf800
#define BLUE 0x001F
#define GREEN 0x07E0
#define MAGENTA 0xF81F
#define WHITE 0xffff
#define BLACK 0x0000
#define YELLOW 0xFFE0

// Function prototypes
void initDisplay();
unsigned long initShow();
void displayText(const char* text, uint16_t fillColor, uint16_t textColor, uint8_t textSize, int x, int y);

#endif

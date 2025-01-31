#include "Checksum.h"

// Function definition
int checksum(String line) {

  // Check the first character
  char firstChar = line[0];
  if (firstChar != 'G' && firstChar != 'C' && firstChar != 'T' && firstChar != 'p' && firstChar != 'L') {
    return 0;  // Return 0 if the first character is not G, C, T, p, or L
  }
  
  // Check if there's a comma on position 2, 3, or 4 (index 1, 2, or 3)
  if (line.indexOf(',', 1) != 1 && line.indexOf(',', 2) != 2 && line.indexOf(',', 3) != 3) {
    return 0;  // Return 0 if there's no comma at position 2, 3, or 4
  }
  
  // Check for non-ASCII characters
  for (int i = 0; i < line.length(); i++) {
    if (line[i] < 0 || line[i] > 127) {
      return 0;  // Return 0 if a non-ASCII character is found
    }
  }
  
  return 1;  // Return 1 if the data is valid
}

#ifndef OLEDDISPLAYP_H_
#define OLEDDISPLAYP_H_

#include "../ProjectDefines.h"
#include <stdint.h>

// Initializes TWI0 (I2C) on PC4/PC5 and wakes up the SSD1306 OLED (tries 0x3C, falls back to 0x3D)
// Returns 0 on success, or the TWI error status code if neither address responded
uint8_t OledDisplayP_Init(void);

// Clears all 8 pages (128x64 pixels) of the OLED display
void OledDisplayP_Clear(void);

// Writes a null-terminated ASCII string on a specific row (page 0 to 7) starting at column x (0 to 127)
void OledDisplayP_PrintLine(uint8_t page, uint8_t col, const char *text);

#endif /* OLEDDISPLAYP_H_ */

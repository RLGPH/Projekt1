#include "../../include/Display/OledDisplayP.h"
#include <avr/pgmspace.h>
#include <util/delay.h>

// Active 7-bit I2C address (automatically detected during Init: 0x3C or 0x3D)
static uint8_t active_oled_addr = 0x3C;

// Compact 5x7 ASCII font stored in Flash (PROGMEM) covering characters ' ' (32) to 'Z' (90)
static const uint8_t font5x7[][5] PROGMEM = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // ' ' (32)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // '!'
    {0x00, 0x07, 0x00, 0x07, 0x00}, // '"'
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // '#'
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // '$'
    {0x23, 0x13, 0x08, 0x64, 0x62}, // '%'
    {0x36, 0x49, 0x55, 0x22, 0x50}, // '&'
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '''
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // '('
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // ')'
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // '*'
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // '+'
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ','
    {0x08, 0x08, 0x08, 0x08, 0x08}, // '-'
    {0x00, 0x60, 0x60, 0x00, 0x00}, // '.'
    {0x20, 0x10, 0x08, 0x04, 0x02}, // '/'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // '0' (48)
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // '6'
    {0x01, 0x71, 0x09, 0x05, 0x03}, // '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // '9'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // ':' (58)
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ';'
    {0x08, 0x14, 0x22, 0x41, 0x00}, // '<'
    {0x14, 0x14, 0x14, 0x14, 0x14}, // '='
    {0x00, 0x41, 0x22, 0x14, 0x08}, // '>'
    {0x02, 0x01, 0x51, 0x09, 0x06}, // '?'
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // '@'
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A' (65)
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 'E'
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // 'F'
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 'G'
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 'H'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 'I'
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 'J'
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 'K'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 'L'
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 'M'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 'N'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 'O'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 'P'
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 'Q'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 'R'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 'T'
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 'U'
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 'V'
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 'X'
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}  // 'Z' (90)
};

// --- Low-level TWI0 (I2C on PC4=SDA0, PC5=SCL0) helper functions ---

static void I2C0_Init(void) {
    PRR0 &= ~(1 << PRTWI0);                  // Power on TWI0 module
    PORTC |= (1 << PORTC4) | (1 << PORTC5);  // Enable internal pull-ups on PC4 & PC5
    TWSR0 = 0x00;                            // Prescaler = 1
    TWBR0 = 12; // 16 MHz / (16 + 2 * 12 * 1) = 400 kHz Fast I2C
    TWCR0 = (1 << TWEN);                     // Enable TWI0 hardware
}

static uint8_t I2C0_Wait(void) {
    uint16_t timeout = 0xFFFF;
    while (!(TWCR0 & (1 << TWINT))) {
        if (--timeout == 0) {
            return 0xFF; // Bus timed out (check SDA/SCL wiring)
        }
    }
    return (TWSR0 & 0xF8); // Return masked 5-bit TWI status code
}

static uint8_t I2C0_Start(uint8_t sla_w) {
    TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send START condition
    uint8_t status = I2C0_Wait();
    if (status != 0x08 && status != 0x10) {
        return status;
    }

    TWDR0 = sla_w;                                     // Load 7-bit address + Write bit (0)
    TWCR0 = (1 << TWINT) | (1 << TWEN);                // Transmit address
    status = I2C0_Wait();
    if (status != 0x18) {                              // 0x18 = SLA+W transmitted, ACK received
        return status;
    }
    return 0; // Success
}

static void I2C0_WriteByte(uint8_t data) {
    TWDR0 = data;
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    I2C0_Wait();
}

static void I2C0_Stop(void) {
    TWCR0 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

static void OLED_SendCommand(uint8_t cmd) {
    if (I2C0_Start(active_oled_addr << 1) == 0) {
        I2C0_WriteByte(0x00); // Control byte: Co = 0, D/C# = 0 (Command)
        I2C0_WriteByte(cmd);
    }
    I2C0_Stop();
}

// --- Public OLED Display Functions ---

uint8_t OledDisplayP_Init(void) {
    I2C0_Init();
    _delay_ms(50); // Give OLED controller time to power up

    // Try standard SSD1306 address 0x3C first; if NACKed, try 0x3D
    uint8_t status = I2C0_Start(0x3C << 1);
    I2C0_Stop();

    if (status == 0) {
        active_oled_addr = 0x3C;
    } else {
        status = I2C0_Start(0x3D << 1);
        I2C0_Stop();
        if (status == 0) {
            active_oled_addr = 0x3D;
        } else {
            return status; // Neither address responded -> return TWI error code
        }
    }

    // Standard 128x64 SSD1306 initialization sequence
    OLED_SendCommand(0xAE); // Display OFF
    OLED_SendCommand(0xD5); OLED_SendCommand(0x80); // Clock divide ratio
    OLED_SendCommand(0xA8); OLED_SendCommand(0x3F); // Multiplex ratio = 64
    OLED_SendCommand(0xD3); OLED_SendCommand(0x00); // Display offset = 0
    OLED_SendCommand(0x40);                         // Start line = 0
    OLED_SendCommand(0x8D); OLED_SendCommand(0x14); // Enable internal charge pump (CRITICAL!)
    OLED_SendCommand(0x20); OLED_SendCommand(0x02); // Page Addressing Mode
    OLED_SendCommand(0xA1);                         // Segment remap (flip horizontally)
    OLED_SendCommand(0xC8);                         // COM scan direction (flip vertically)
    OLED_SendCommand(0xDA); OLED_SendCommand(0x12); // COM pins hardware config
    OLED_SendCommand(0x81); OLED_SendCommand(0xCF); // Contrast
    OLED_SendCommand(0xD9); OLED_SendCommand(0xF1); // Pre-charge period
    OLED_SendCommand(0xDB); OLED_SendCommand(0x40); // VCOMH deselect level
    OLED_SendCommand(0xA4);                         // Resume to RAM content display
    OLED_SendCommand(0xA6);                         // Normal display (non-inverted)
    OLED_SendCommand(0xAF);                         // Display ON

    OledDisplayP_Clear();
    return 0; // Success!
}

void OledDisplayP_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_SendCommand(0xB0 + page); // Set page address (0..7)
        OLED_SendCommand(0x00);        // Set lower column start address
        OLED_SendCommand(0x10);        // Set higher column start address

        if (I2C0_Start(active_oled_addr << 1) == 0) {
            I2C0_WriteByte(0x40); // Control byte: Co = 0, D/C# = 1 (Data)
            for (uint8_t col = 0; col < 128; col++) {
                I2C0_WriteByte(0x00); // Clear all 8 vertical pixels in this column
            }
        }
        I2C0_Stop();
    }
}

void OledDisplayP_PrintLine(uint8_t page, uint8_t col, const char *text) {
    if (page > 7 || col > 127) return;

    // 1. Clear the entire page first so leftover digits are erased
    OLED_SendCommand(0xB0 + page);
    OLED_SendCommand(0x00);
    OLED_SendCommand(0x10);
    if (I2C0_Start(active_oled_addr << 1) == 0) {
        I2C0_WriteByte(0x40);
        for (uint8_t c = 0; c < 128; c++) {
            I2C0_WriteByte(0x00);
        }
    }
    I2C0_Stop();

    // 2. Set starting column and write each character
    OLED_SendCommand(0xB0 + page);
    OLED_SendCommand(col & 0x0F);
    OLED_SendCommand(0x10 | ((col >> 4) & 0x0F));

    if (I2C0_Start(active_oled_addr << 1) == 0) {
        I2C0_WriteByte(0x40);
        for (uint8_t i = 0; text[i] != '\0'; i++) {
            char ch = text[i];
            // Automatically convert lowercase 'a'..'z' to uppercase 'A'..'Z' for our compact font
            if (ch >= 'a' && ch <= 'z') {
                ch -= 32;
            }
            if (ch < 32 || ch > 90) {
                ch = ' ';
            }
            uint8_t idx = ch - 32;
            for (uint8_t b = 0; b < 5; b++) {
                I2C0_WriteByte(pgm_read_byte(&font5x7[idx][b]));
            }
            I2C0_WriteByte(0x00); // 1-pixel spacing between characters
        }
    }
    I2C0_Stop();
}
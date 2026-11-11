//#define F_CPU 16000000UL  // Antager 16 MHz krystal (ret hvis din kører med f.eks. 8 MHz)

//#include <avr/io.h>
//#include <avr/interrupt.h>

//#define GreenLedBit PORTB1 
#include "../ProjectDefines.h"
#include <stdint.h>

extern void Timer1_Init_ms(uint16_t timeout_ms);
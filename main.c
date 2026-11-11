/**
 * @file main.c
 * @author ltpe
 * @date 2026-11-05
 * @brief Main function
 */

#include "include/ProjectDefines.h"
#include "include/Timer/Timer.h"
#include "include/UART/Uart.h"
#include <stdint.h>

volatile uint16_t timer_flag = 0;
uint32_t seconds_timer = 1200;

void ConvertReceivedCharMain(char *ReceivedChar)
{
	// Den smarte måde at få konverteret små bogstaver om til store bogstaver og
	// modsat er ved brug af Xor, som vi tidligere har set. Så kan vi klare det i
	// én linje kode.
	if ( ((*ReceivedChar >= 0x41) && (*ReceivedChar <= 0x5D)) ||
	     ((*ReceivedChar >= 0x61) && (*ReceivedChar <= 0x7D)))
	{
		*ReceivedChar = *ReceivedChar & (~Upper_Lower_Bit_Value);
	}
}

int main(void) {
    DDRB |= (1 << GreenLedBit);  // Sæt PB1 som output for den grønne LED

    Timer1_Init_ms(1000);

    RS232Init();
    RS232Init_Uart1(); // Initialiserer UART1 (til CP2102)
    Enable_UART_Receive_Interrupt();

    // Aktivér globale interrupts
    sei();

    while (1) {
        if (timer_flag == 1) {
            seconds_timer++; 
            
            printf("Sekunder efter opstart: %lu\n", seconds_timer); 
            
            timer_flag = 0; 
        }
    }
}
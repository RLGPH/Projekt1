#include "include/ProjectDefines.h"
#include "include/Timer/Timer.h"
#include "include/UART/Uart.h"
#include "include/ExternInteruptP/ExternInteruptP.h"
#include "include/Display/OledDisplayP.h"
#include <stdint.h>
#include <stdio.h>

volatile uint8_t timer_flag = 0;

int main(void) {
    uint32_t seconds_counter = 0;

    DDRB |= (1 << GreenLedBit); // PB1 as output for the 1-second blinking LED

    // 1. Initialize Timer1 for 1000 ms (1 Hz) interrupts (Requirement 5)
    Timer1_Init_ms(1000);

    // 2. Initialize UART0 and UART1 + enable RX interrupts on BOTH (Requirements 6 & 7)
    RS232Init();
    RS232Init_Uart1();
    Enable_UART_Receive_Interrupt();
    Enable_UART1_Receive_Interrupt();

    // 3. Initialize External Interrupt 0 on PD2 (Requirements 1, 2, 3)
    ExternInteruptP_Init();

    // 4. Initialize OLED Display and show initial counter (Requirement 4)
    OledDisplayP_Init();
    OledDisplayP_PrintLine(0, 0, "EXT INT0 COUNTER");
    OledDisplayP_PrintLine(2, 0, "COUNT: 0");

    // 5. Enable global interrupts
    sei();

    while (1) {
        // Handle External Interrupt 0 button press events (UART0, UART1, and OLED)
        ExternInteruptP_Process();

        // Requirement 5: Every 1 second, send "Sekund" value out on UART1
        if (timer_flag == 1) {
            timer_flag = 0;
            seconds_counter++;

            char sec_msg[32];
            snprintf(sec_msg, sizeof(sec_msg), "Sekund: %lu\r\n", seconds_counter);

            for (uint8_t i = 0; sec_msg[i] != '\0'; i++) {
                uart1_putch(sec_msg[i], NULL);
            }
        }
    }
}

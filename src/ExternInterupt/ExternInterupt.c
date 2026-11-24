#include "../../include/ExternInteruptP/ExternInteruptP.h"
#include "../../include/UART/Uart.h"
#include "../../include/Display/OledDisplayP.h"
#include "../../include/RingBuffer/RingBuffer.h"
#include <stdio.h>

// 1 = waiting for rising edge, 0 = waiting for falling edge
static volatile uint8_t waiting_for_rising_edge = 1;
static volatile uint8_t update_ext_display_flag = 0;

static volatile uint32_t down_int_counter = 0; // Falling edge count
static volatile uint32_t up_int_counter = 0;   // Rising edge count

void ExternInteruptP_Init(void) {
    // Configure YellowLedBit (PB2) as output for External Interrupt toggle
    DDRB |= (1 << YellowLedBit);

    // Configure PD2 (INT0) as input (external pull-down on button board)
    DDRD &= ~(1 << DDD2);
    PORTD &= ~(1 << PORTD2);

    // Start by triggering on RISING edge (ISC01 = 1, ISC00 = 1)
    EICRA |= (1 << ISC01) | (1 << ISC00);
    EIFR |= (1 << INTF0);
    EIMSK |= (1 << INT0);
}

ISR(INT0_vect) {
    // Toggle LED on every external interrupt (Opgave 5 Krav 10)
    PORTB ^= (1 << YellowLedBit);

    if (waiting_for_rising_edge == 1) {
        up_int_counter++;
        waiting_for_rising_edge = 0;

        // Switch to FALLING edge next (ISC01 = 1, ISC00 = 0)
        EICRA |= (1 << ISC01);
        EICRA &= ~(1 << ISC00);
    } else {
        down_int_counter++;
        waiting_for_rising_edge = 1;

        // Switch back to RISING edge next (ISC01 = 1, ISC00 = 1)
        EICRA |= (1 << ISC01) | (1 << ISC00);
    }

    update_ext_display_flag = 1;
}

void ExternInteruptP_Process(void) {
    if (update_ext_display_flag) {
        update_ext_display_flag = 0;

        uint32_t down_copy, up_copy;
        uint8_t sreg = SREG;
        cli();
        down_copy = down_int_counter;
        up_copy = up_int_counter;
        SREG = sreg;

        // Opgave 4 Krav 3: Print interrupt count to UART0 (and UART1 when Ringbuffer test is idle)
        char msg[48];
        snprintf(msg, sizeof(msg), "\r\nExternal Interrupt Count (UP:%lu DOWN:%lu)\r\n", up_copy, down_copy);
        for (uint8_t i = 0; msg[i] != '\0'; i++) {
            uart_putch(msg[i], NULL);
            if (!RingBuffer_IsRunning()) {
                uart1_putch(msg[i], NULL);
            }
        }

        // Opgave 5 Krav 11 & 12: Update OLED Line 0 & Line 1
        char line_buf[24];
        snprintf(line_buf, sizeof(line_buf), "# DOWN INT: %lu", down_copy);
        OledDisplayP_PrintLine(0, 0, line_buf);

        snprintf(line_buf, sizeof(line_buf), "# UP INT:   %lu", up_copy);
        OledDisplayP_PrintLine(1, 0, line_buf);
    }
}

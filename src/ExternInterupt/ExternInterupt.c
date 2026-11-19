#include "../../include/ExternInteruptP/ExternInteruptP.h"
#include "../../include/UART/Uart.h"
#include "../../include/Display/OledDisplayP.h"
#include <stdio.h>

// Tracks which edge we are currently waiting for
// 1 = waiting for rising edge (button press: 0V -> 5V)
// 0 = waiting for falling edge (button release: 5V -> 0V)
static volatile uint8_t waiting_for_rising_edge = 1;

// Flag set by the ISR to notify main() that a valid press occurred
static volatile uint8_t valid_press_flag = 0;

// Counter for total valid button presses
static uint32_t ext_int_counter = 0;

void ExternInteruptP_Init(void) {
    // 1. Configure PD2 (INT0) as an input
    DDRD &= ~(1 << DDD2);
    
    // Make sure internal pull-up is OFF since we have an external 1k pull-down resistor
    PORTD &= ~(1 << PORTD2);

    // 2. Configure EICRA to trigger INT0 on a RISING edge (ISC01 = 1, ISC00 = 1)
    EICRA |= (1 << ISC01) | (1 << ISC00);

    // 3. Clear any pending interrupt flag on INT0 before enabling it
    EIFR |= (1 << INTF0);

    // 4. Enable External Interrupt 0 in the External Interrupt Mask Register
    EIMSK |= (1 << INT0);
}

// Interrupt Service Routine for External Interrupt 0 (PORTD2)
ISR(INT0_vect) {
    if (waiting_for_rising_edge == 1) {
        // Button was pressed (rising edge: 0V -> 5V)
        valid_press_flag = 1;
        waiting_for_rising_edge = 0;

        // Switch EICRA to trigger on FALLING edge next (ISC01 = 1, ISC00 = 0)
        EICRA |= (1 << ISC01);
        EICRA &= ~(1 << ISC00);
    } 
    else {
        // Button was released (falling edge: 5V -> 0V)
        // Do NOT count this edge; reset back to waiting for a rising edge
        waiting_for_rising_edge = 1;

        // Switch EICRA back to trigger on RISING edge next (ISC01 = 1, ISC00 = 1)
        EICRA |= (1 << ISC01) | (1 << ISC00);
    }
}

void ExternInteruptP_Process(void) {
    if (valid_press_flag == 1) {
        valid_press_flag = 0; // Reset the flag immediately
        ext_int_counter++;    // Increment the valid interrupt counter

        // 1. Send the message out on BOTH UART0 and UART1 (Requirement 3)
        char msg[48];
        snprintf(msg, sizeof(msg), "\r\nExternal Interrupt Count: %lu\r\n", ext_int_counter);

        for (uint8_t i = 0; msg[i] != '\0'; i++) {
            uart_putch(msg[i], NULL);   // Send to UART0 (Direct USB)
            uart1_putch(msg[i], NULL);  // Send to UART1 (CP2102)
        }

                char display_msg[24];
        snprintf(display_msg, sizeof(display_msg), "COUNT: %lu", ext_int_counter);
        OledDisplayP_PrintLine(2, 0, display_msg);

    }
}

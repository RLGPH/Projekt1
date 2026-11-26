#include "include/ProjectDefines.h"
#include "include/Timer/Timer.h"
#include "include/UART/Uart.h"
#include "include/Statemachine/Statemachine.h"
#include "include/ExternInteruptP/ExternInteruptP.h"
#include "include/Display/OledDisplayP.h"
#include "include/RingBuffer/RingBuffer.h"
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>

volatile uint8_t timer_flag = 0;

// EEPROM storage for Opgave 5 Krav 17 (32-bit runtime in minutes + magic signature)
static uint32_t EEMEM ee_magic_signature = 0xDEADBEEF;
static uint32_t EEMEM ee_runtime_minutes = 0;

static uint32_t LoadRuntimeSecondsFromEEPROM(void) {
    uint32_t magic = eeprom_read_dword(&ee_magic_signature);
    if (magic != 0xDEADBEEF) {
        eeprom_update_dword(&ee_magic_signature, 0xDEADBEEF);
        eeprom_update_dword(&ee_runtime_minutes, 0);
        return 0;
    }
    uint32_t saved_minutes = eeprom_read_dword(&ee_runtime_minutes);
    return saved_minutes * 60UL;
}

int main(void) {
    // PB1 (GreenLedBit) as output for 1-second Timer1 LED toggle
    DDRB |= (1 << GreenLedBit);

    // Load previously saved runtime from EEPROM
    uint32_t total_seconds = LoadRuntimeSecondsFromEEPROM();
    uint8_t sub_minute_seconds = 0;

    // 1. Initialize Timer1 for 1000 ms (1 Hz) interrupts (Opgave 1, 2, 4, 5)
    Timer1_Init_ms(1000);

    // 2. Initialize UART0 and UART1 + enable RX interrupts on BOTH
    RS232Init();
    RS232Init_Uart1();
    Enable_UART_Receive_Interrupt();
    Enable_UART1_Receive_Interrupt();

    // 3. Connect Opgave 3 StateMachine callback + initialize StateMachine & RingBuffer
    SetupUARTFunctionCallbackPointer(StateMachine_OnUARTCharReceived);
    StateMachine_Init();
    RingBuffer_Init();

    // 4. Initialize External Interrupt 0 on PD2 (Opgave 4 & 5)
    ExternInteruptP_Init();

    // 5. Initialize SSD1306 OLED and draw screen layout (Opgave 4 & 5)
    OledDisplayP_Init();
    OledDisplayP_PrintLine(0, 0, "# DOWN INT: 0");
    OledDisplayP_PrintLine(1, 0, "# UP INT:   0");

    char time_buf[24];
    snprintf(time_buf, sizeof(time_buf), "TIME: %lu S", total_seconds);
    OledDisplayP_PrintLine(2, 0, time_buf);

    OledDisplayP_PrintLine(3, 0, "--- --- --- --- ---");
    OledDisplayP_PrintLine(4, 0, "USER: " USER_NAME);
    OledDisplayP_PrintLine(5, 0, "EEPROM: 32-BIT MIN");
    OledDisplayP_PrintLine(6, 0, "BAUD: 115200");
    OledDisplayP_PrintLine(7, 0, "EMBEDDED STYRER !!!");

    // 6. Enable global interrupts
    sei();

        while (1) {
        // Opgave 5: Process RingBuffer handshake, overrun reporting, and byte echoing
        RingBuffer_Process();

        // Opgave 3: Process Register/Bit StateMachine when RingBuffer test is idle
        if (!RingBuffer_IsRunning()) {
            StateMachine_Process();
        }

        // Opgave 4 & 5: Process External Interrupt button events (UART0, UART1, LED, OLED)
        ExternInteruptP_Process();

        // Opgave 2, 4 & 5: Process 1-second Timer1 tick
        if (timer_flag == 1) {
            timer_flag = 0;
            total_seconds++;
            sub_minute_seconds++;

            // Save every 60 seconds to EEPROM (Opgave 5 Krav 17)
            if (sub_minute_seconds >= 60) {
                sub_minute_seconds = 0;
                uint32_t total_minutes = total_seconds / 60UL;
                eeprom_update_dword(&ee_runtime_minutes, total_minutes);
            }

            // Opgave 5 Krav 13: Update active time in seconds on OLED Line 2
            snprintf(time_buf, sizeof(time_buf), "TIME: %lu S", total_seconds);
            OledDisplayP_PrintLine(2, 0, time_buf);

            // Drain any incoming UART bytes so OLED I2C updates never delay sequence completion
            RingBuffer_Process();

            // Opgave 5 Krav 15: Update last 5 RingBuffer bytes on OLED Line 3
            uint8_t last5[5];
            uint8_t count = 0;
            RingBuffer_GetLast5(last5, &count);

            char rb_line[24];
            if (count == 0) {
                snprintf(rb_line, sizeof(rb_line), "--- --- --- --- ---");
            } else {
                snprintf(rb_line, sizeof(rb_line), "%03u %03u %03u %03u %03u",
                         last5[0], last5[1], last5[2], last5[3], last5[4]);
            }
            OledDisplayP_PrintLine(3, 0, rb_line);
        }
    }
}

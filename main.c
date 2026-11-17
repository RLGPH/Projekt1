/**
 * @file main.c
 * @author ltpe
 * @date 2026-11-05
 * @brief Main function
 */

#include "include/ProjectDefines.h"
#include "include/Timer/Timer.h"
#include "include/UART/Uart.h"
#include "include/Statemachine/Statemachine.h" 
#include <stdint.h>

volatile uint8_t timer_flag = 0; // Rettet til uint8_t så den matcher Timer.c

int main(void) {
    DDRB |= (1 << GreenLedBit);  // Sæt PB1 som output for den grønne LED

    Timer1_Init_ms(1000);

    RS232Init();
    RS232Init_Uart1(); // Initialiserer UART1 (til CP2102)
    
    // Registrer tilstandsmaskinens callback-funktion til UART receive interrupt
    SetupUARTFunctionCallbackPointer(StateMachine_OnUARTCharReceived);
    Enable_UART_Receive_Interrupt();

    // Aktivér globale interrupts
    sei();

    // Start tilstandsmaskinen og udskriv første tilstand
    StateMachine_Init();

    while (1) {
        StateMachine_Process();

        if (timer_flag == 1) {
            timer_flag = 0; 
        }
    }
}

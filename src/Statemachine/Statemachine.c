#include "../../include/Statemachine/Statemachine.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// Private variabler (static så kun StateMachine.c har adgang til dem)
static volatile char rx_char = 0;
static volatile uint8_t new_char_received = 0;

static State_t current_state = Modtag_Adresse;
static char addr_buffer[10]; // Antag at adresse ikke overstiger 9 tegn
static uint8_t addr_index = 0;

static volatile uint8_t *target_address = NULL;
static uint8_t bit_position = 0;
static uint8_t bit_value = 0;

// Privat hjælpefunktion til at skifte tilstand og udskrive besked på UART (Punkt 6)
static void ChangeState(State_t new_state) {
    current_state = new_state;
    switch (current_state) {
        case Modtag_Adresse:
            addr_index = 0;
            printf("\r\n[Tilstand: Modtag_Adresse] Indtast hex-adresse (f.eks. 0025 eller 0x0025) afsluttet med ':' -> ");
            break;
        case Modtag_Bit_Position_I_Adresse:
            printf("\r\n[Tilstand: Modtag_Bit_Position_I_Adresse] Indtast bit position (0-7) -> ");
            break;
        case Modtag_Bit_Positon_Vaerdi:
            printf("\r\n[Tilstand: Modtag_Bit_Positon_Vaerdi] Indtast bit vaerdi (0 eller 1) -> ");
            break;
    }
}

// Privat hjælpefunktion til at udskrive en byte som 8 bits f.eks. "0000 1111" (Punkt 5, Linje 4)
static void PrintBinaryByte(uint8_t val) {
    for (int8_t i = 7; i >= 0; i--) {
        printf("%c", (val & (1 << i)) ? '1' : '0');
        if (i == 4) {
            printf(" "); // Mellemrum mellem de to nibbles
        }
    }
}

// Initialiserer tilstandsmaskinen og udskriver starttilstand
void StateMachine_Init(void) {
    new_char_received = 0;
    ChangeState(Modtag_Adresse);
}

// Callback-funktion som kaldes af UART receive interrupt
void StateMachine_OnUARTCharReceived(char *ReceivedChar) {
    rx_char = *ReceivedChar;
    new_char_received = 1;
}

// Kører selve tilstandsmaskinen når der er modtaget et nyt tegn
void StateMachine_Process(void) {
    if (!new_char_received) {
        return; // Intet nyt tegn at behandle
    }

    char ch = rx_char;
    new_char_received = 0;

    // Ignorer linjeskift (CR og LF) hvis terminalen sender dem automatisk
    if (ch == '\r' || ch == '\n') {
        return;
    }

    switch (current_state) {
        case Modtag_Adresse:
            if (ch == ':') {
                if (addr_index > 0) {
                    addr_buffer[addr_index] = '\0';
                    // Konverter hex-streng (f.eks. "0x0025" eller "0025") til memory-adresse
                    uint16_t parsed_addr = (uint16_t)strtoul(addr_buffer, NULL, 16);
                    target_address = (volatile uint8_t *)(uintptr_t)parsed_addr;
                    ChangeState(Modtag_Bit_Position_I_Adresse);
                } else {
                    printf("\r\nFejl: Ingen adresse indtastet.");
                    ChangeState(Modtag_Adresse);
                }
            } 
            else if (isxdigit((unsigned char)ch) || ch == 'x' || ch == 'X') {
                if (addr_index < sizeof(addr_buffer) - 1) {
                    addr_buffer[addr_index++] = ch;
                } else {
                    printf("\r\nFejl: Adresse er for lang.");
                    ChangeState(Modtag_Adresse);
                }
            } 
            else {
                printf("\r\nFejl: Ugyldigt tegn i adresse.");
                ChangeState(Modtag_Adresse);
            }
            break;

        case Modtag_Bit_Position_I_Adresse:
            if (ch >= '0' && ch <= '7') {
                bit_position = ch - '0';
                ChangeState(Modtag_Bit_Positon_Vaerdi);
            } else {
                printf("\r\nFejl: Ugyldig bit-position (skal vaere 0-7).");
                ChangeState(Modtag_Adresse);
            }
            break;

        case Modtag_Bit_Positon_Vaerdi:
            if (ch == '0' || ch == '1') {
                bit_value = ch - '0';

                // Sæt eller clear kun den udpegede bit via pointeren (Punkt 5)
                if (bit_value == 1) {
                    *target_address |= (1 << bit_position);
                } else {
                    *target_address &= ~(1 << bit_position);
                }

                // Udskriv de 4 påkrævede linjer på UART (Punkt 5)
                printf("\r\n--- Resultat ---\r\n");
                printf("Linje 1 - Adresse: 0x%04X\r\n", (uint16_t)(uintptr_t)target_address);
                printf("Linje 2 - Bit position: %u\r\n", bit_position);
                printf("Linje 3 - Bit vaerdi: %u\r\n", bit_value);
                printf("Linje 4 - Bit moenster: ");
                PrintBinaryByte(*target_address);
                printf("\r\n----------------\r\n");

                // Start forfra i første tilstand
                ChangeState(Modtag_Adresse);
            } else {
                printf("\r\nFejl: Ugyldig bit-vaerdi (skal vaere 0 eller 1).");
                ChangeState(Modtag_Adresse);
            }
            break;
    }
}

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include <stdint.h>

typedef enum {
    Modtag_Adresse,
    Modtag_Bit_Position_I_Adresse,
    Modtag_Bit_Positon_Vaerdi
} State_t;

// Funktioner tilgængelige for main.c
void StateMachine_Init(void);
void StateMachine_OnUARTCharReceived(char *ReceivedChar);
void StateMachine_Process(void);

#endif /* STATEMACHINE_H_ */

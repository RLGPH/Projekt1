#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include "../ProjectDefines.h"

typedef void (*Function_Pointer_With_One_CharPointer_Parameter)(char*);

#define BAUD 115200UL

#ifdef UART_NORMAL_MODE
#define MyUBRR (unsigned int)(F_CPU/(16*BAUD) - 1)
#else
#define MyUBRR (unsigned int)(F_CPU/(8*BAUD) - 1)
#endif

extern void RS232Init_Uart1(void);
extern int uart1_putch(char ch, FILE* stream);
extern void RS232Init(void);
extern int uart_getch(FILE* stream);
extern int uart_putch(char ch, FILE* stream);
extern void Enable_UART_Receive_Interrupt(void);
extern void Disable_UART_Receive_Interupt(void);
extern void Enable_UART1_Receive_Interrupt(void);

void SetupUARTFunctionCallbackPointer(Function_Pointer_With_One_CharPointer_Parameter Function_To_Callback);

#endif /* UART_H_ */

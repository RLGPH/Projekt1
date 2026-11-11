/*
 * Uart.c
 *
 * Created: 15-01-2018 09:44:48
 *  Author: ltpe
 */ 

#include "../../include/UART/Uart.h"

// 1. Opretter output-stream til UART1, som peger på vores rettede uart1_putch funktion
static FILE uart1_output = FDEV_SETUP_STREAM(uart1_putch, NULL, _FDEV_SETUP_WRITE);

static FILE uart_output = FDEV_SETUP_STREAM(uart_putch, NULL, _FDEV_SETUP_WRITE);
static FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getch, _FDEV_SETUP_READ);

static Function_Pointer_With_One_CharPointer_Parameter Callback_Function_Pointer = NULL;
 
void SetupUARTFunctionCallbackPointer(Function_Pointer_With_One_CharPointer_Parameter Function_To_Callback)
{
	Callback_Function_Pointer = Function_To_Callback;
}

void ConvertReceivedChar(char *ReceivedChar)
{
	if ( ((*ReceivedChar >= 0x41) && (*ReceivedChar <= 0x5D)) ||
	     ((*ReceivedChar >= 0x61) && (*ReceivedChar <= 0x7D)))
	{
		*ReceivedChar = *ReceivedChar ^ Upper_Lower_Bit_Value;
	}
}

// Initialisering af UART1 (CP2102 på PB3 og PB4)
void RS232Init_Uart1(void)
{
    // TÆND FOR STRØMMEN TIL UART1 HARDWAREN!
    // Vi rydder PRUSART1-bitten i Power Reduction Register 1 for at tænde modulet.
    PRR1 &= ~(1 << PRUSART1);

    // Sæt baud rate til 9600 på UART1
    UBRR1L = (unsigned char)(MyUBRR & 0xff);
    UBRR1H = (unsigned char)(MyUBRR >> 8);
    
    #ifdef UART_NORMAL_MODE
    UCSR1A = 0x00;
    #else
    UCSR1A = (1 << U2X); 
    #endif
    
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);    // Tænd for Rx og Tx på UART1
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);  // 8 data bit, 1 stop bit
    
    // NU omdirigerer vi stdout, så printf() sender direkte til UART1!
    stdout = &uart1_output;
}

// RETTET: Denne funktion skal kigge i UCSR1A kontrolregistret for UART1!
int uart1_putch(char ch, FILE* stream)
{
    while( !(UCSR1A & (1 << UDRE1))) ; // Tjekker nu det RIGTIGE register (UCSR1A)
    UDR1 = ch;                       
    return 0;
}

// Initialisering af UART0 (Direkte USB / ttyACM0)
void RS232Init(void)
{
	 UBRR0L = (unsigned char)(MyUBRR & 0xff);
	 UBRR0H = (unsigned char)(MyUBRR >> 8);
	 #ifdef UART_NORMAL_MODE
	 UCSR0A = 0x00;
	 #else
	 UCSR0A = 1 << U2X; 
	 #endif
	 UCSR0B = (1 << RXEN0) | (1 << TXEN0);  
	 UCSR0C = (1 << UCSZ11) | (1 << UCSZ10); 
	 stdout = &uart_output;
	 stdin = &uart_input;
}

void Enable_UART_Receive_Interrupt()
{
	 UCSR0B |= (1 << RXCIE0); 
}

void Disable_UART_Receive_Interupt()
{
	UCSR0B &= ~(1 << RXCIE0); 
}

int uart_getch(FILE* stream)
{
	 while( !(UCSR0A & (1 << RXC0))) ; 
	 return(UDR0);
}

int uart_putch(char ch, FILE* stream)
{
	 while( !(UCSR0A & (1 << UDRE0))) ;
	 UDR0 = ch;
	 return 0;
}

// UART0 modtage-interrupt: Ekkoer og konverterer til BÅDE UART0 og UART1
ISR(USART0_RX_vect)
{
    char ReceivedByte;
    ReceivedByte = UDR0; 
    
    ConvertReceivedChar(&ReceivedByte); 
    
    UDR0 = ReceivedByte;     // Udskriv på UART0 (Direct USB)
    UDR1 = ReceivedByte;     // Udskriv på UART1 (CP2102)
}


 //ISR(USART0_RX_vect)
 //{
	// char ReceivedByte;
	//ReceivedByte = UDR0;
	//ConvertReceivedChar(&ReceivedByte);
	 //Callback_Function_Pointer(&ReceivedByte); 
	 //ReceiveNewTimeoutValue(&ReceivedByte);
	//UDR0 = ReceivedByte; // Echo back the received byte to the computer
//}
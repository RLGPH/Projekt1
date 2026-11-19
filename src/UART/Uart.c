#include "../../include/UART/Uart.h"

static FILE uart1_output = FDEV_SETUP_STREAM(uart1_putch, NULL, _FDEV_SETUP_WRITE);
static FILE uart_output = FDEV_SETUP_STREAM(uart_putch, NULL, _FDEV_SETUP_WRITE);
static FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getch, _FDEV_SETUP_READ);

static Function_Pointer_With_One_CharPointer_Parameter Callback_Function_Pointer = NULL;
 
void SetupUARTFunctionCallbackPointer(Function_Pointer_With_One_CharPointer_Parameter Function_To_Callback)
{
    Callback_Function_Pointer = Function_To_Callback;
}

// Helper: Forces ASCII letters 'a'-'z' to UPPERCASE 'A'-'Z' (Requirement 6)
static char ToUpperCase(char ch)
{
    if (ch >= 'a' && ch <= 'z') {
        ch -= 32; // Clear bit 5 (0x20) to convert lowercase to uppercase
    }
    return ch;
}

// Helper: Forces ASCII letters 'A'-'Z' to lowercase 'a'-'z' (Requirement 7)
static char ToLowerCase(char ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        ch += 32; // Set bit 5 (0x20) to convert uppercase to lowercase
    }
    return ch;
}

// Initialization of UART1 (CP2102 on PB3 and PB4)
void RS232Init_Uart1(void)
{
    // Power on USART1 module in Power Reduction Register 1
    PRR1 &= ~(1 << PRUSART1);

    // Set baud rate to 9600 on UART1
    UBRR1L = (unsigned char)(MyUBRR & 0xff);
    UBRR1H = (unsigned char)(MyUBRR >> 8);
    
    #ifdef UART_NORMAL_MODE
    UCSR1A = 0x00;
    #else
    UCSR1A = (1 << U2X); 
    #endif
    
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);    // Enable Rx and Tx on UART1
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);  // 8 data bits, 1 stop bit
    
    stdout = &uart1_output;
}

int uart1_putch(char ch, FILE* stream)
{
    while (!(UCSR1A & (1 << UDRE1))); // Wait until UART1 transmit buffer is empty
    UDR1 = ch;
    return 0;
}

// Initialization of UART0 (Direct USB / mEDBG)
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

void Enable_UART_Receive_Interrupt(void)
{
    UCSR0B |= (1 << RXCIE0); 
}

void Disable_UART_Receive_Interupt(void)
{
    UCSR0B &= ~(1 << RXCIE0); 
}

void Enable_UART1_Receive_Interrupt(void)
{
    UCSR1B |= (1 << RXCIE1); // Enable Receive Complete Interrupt on UART1
}

int uart_getch(FILE* stream)
{
    while (!(UCSR0A & (1 << RXC0))); 
    return (UDR0);
}

int uart_putch(char ch, FILE* stream)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = ch;
    return 0;
}

// Requirement 6: UART0 Receive Interrupt -> Always convert letters to UPPERCASE and echo to UART0
ISR(USART0_RX_vect)
{
    char ReceivedByte = UDR0;
    ReceivedByte = ToUpperCase(ReceivedByte);
    UDR0 = ReceivedByte;
}

// Requirement 7: UART1 Receive Interrupt -> Always convert letters to lowercase and echo to UART1
ISR(USART1_RX_vect)
{
    char ReceivedByte = UDR1;
    ReceivedByte = ToLowerCase(ReceivedByte);
    UDR1 = ReceivedByte;
}

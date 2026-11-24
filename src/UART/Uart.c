#include "../../include/UART/Uart.h"
#include "../../include/RingBuffer/RingBuffer.h"

static FILE uart1_output = FDEV_SETUP_STREAM(uart1_putch, NULL, _FDEV_SETUP_WRITE);
static FILE uart_output = FDEV_SETUP_STREAM(uart_putch, NULL, _FDEV_SETUP_WRITE);
static FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getch, _FDEV_SETUP_READ);

static Function_Pointer_With_One_CharPointer_Parameter Callback_Function_Pointer = NULL;

void SetupUARTFunctionCallbackPointer(Function_Pointer_With_One_CharPointer_Parameter Function_To_Callback)
{
    Callback_Function_Pointer = Function_To_Callback;
}

static char ToLowerCase(char ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        ch += 32;
    }
    return ch;
}

void RS232Init_Uart1(void)
{
    PRR1 &= ~(1 << PRUSART1);

    UBRR1L = (unsigned char)(MyUBRR & 0xff);
    UBRR1H = (unsigned char)(MyUBRR >> 8);

    #ifdef UART_NORMAL_MODE
    UCSR1A = 0x00;
    #else
    UCSR1A = (1 << U2X1); 
    #endif

    UCSR1B = (1 << RXEN1) | (1 << TXEN1);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

int uart1_putch(char ch, FILE* stream)
{
    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = ch;
    return 0;
}

void RS232Init(void)
{
    UBRR0L = (unsigned char)(MyUBRR & 0xff);
    UBRR0H = (unsigned char)(MyUBRR >> 8);
    #ifdef UART_NORMAL_MODE
    UCSR0A = 0x00;
    #else
    UCSR0A = (1 << U2X0); 
    #endif
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);  
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); 
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
    UCSR1B |= (1 << RXCIE1);
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

ISR(USART0_RX_vect)
{
    char ReceivedByte = UDR0;

    // 1. Feed Ringbuffer handshake & stream (UART0 RX -> UART1 TX)
    RingBuffer_OnRxByte((uint8_t)ReceivedByte);

    // 2. Feed Opgave 3 StateMachine callback when Ringbuffer test is idle
    if (!RingBuffer_IsRunning() && Callback_Function_Pointer != NULL) {
        Callback_Function_Pointer(&ReceivedByte);
    }
}

ISR(USART1_RX_vect)
{
    char ReceivedByte = UDR1;
    // Opgave 4: Convert uppercase to lowercase on UART1 RX when Ringbuffer test is idle
    if (!RingBuffer_IsRunning()) {
        ReceivedByte = ToLowerCase(ReceivedByte);
        UDR1 = ReceivedByte;
    }
}

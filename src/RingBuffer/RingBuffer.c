#include "../../include/RingBuffer/RingBuffer.h"
#include "../../include/UART/Uart.h"
#include <stdio.h>

// Matches SerialStressTester1 defaults: RX from UART0 (/dev/ttyACM0), TX to UART1 (/dev/ttyUSB0)
#define TX_SEND_BYTE(b) uart1_putch((char)(b), NULL)

typedef enum {
    RB_STATE_WAIT_FOR_COLON,
    RB_STATE_WAIT_FOR_SEMICOLON,
    RB_STATE_RUNNING
} RbState_t;

static volatile RbState_t rb_state = RB_STATE_WAIT_FOR_COLON;
static volatile uint8_t send_username_flag = 0;

static volatile uint8_t rb_buffer[RINGBUFFER_SIZE];
static volatile uint8_t rb_head = 0;
static volatile uint8_t rb_tail = 0;

// History of the last 5 received bytes for OLED display (Line 3)
static volatile uint8_t last5_buf[5] = {0};
static volatile uint8_t last5_count = 0;

// Overrun tracking
static volatile uint8_t overrun_flag = 0;
static volatile uint8_t overrun_lost_byte = 0;

static void RingBuffer_ResetPointers(void) {
    rb_head = 0;
    rb_tail = 0;
    last5_count = 0;
    overrun_flag = 0;
}

static void SendStringTx(const char *str) {
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        TX_SEND_BYTE(str[i]);
    }
}

void RingBuffer_Init(void) {
    RingBuffer_ResetPointers();
    rb_state = RB_STATE_WAIT_FOR_COLON;
    send_username_flag = 0;
}

uint8_t RingBuffer_IsRunning(void) {
    return (rb_state != RB_STATE_WAIT_FOR_COLON);
}

void RingBuffer_OnRxByte(uint8_t byte) {
    switch (rb_state) {
        case RB_STATE_WAIT_FOR_COLON:
            if (byte == ':') {
                send_username_flag = 1;
                rb_state = RB_STATE_WAIT_FOR_SEMICOLON;
            }
            break;

        case RB_STATE_WAIT_FOR_SEMICOLON:
            if (byte == ';') {
                RingBuffer_ResetPointers();
                rb_state = RB_STATE_RUNNING;
            } else if (byte == ':') {
                send_username_flag = 1;
            }
            break;

        case RB_STATE_RUNNING:
            // Reset back to handshake if PC reports sync error ('_') or overrun ack ('-')
            if (byte == '_' || byte == '-') {
                RingBuffer_ResetPointers();
                rb_state = RB_STATE_WAIT_FOR_COLON;
                return;
            }

            // Record into last-5 history
            for (uint8_t i = 0; i < 4; i++) {
                last5_buf[i] = last5_buf[i + 1];
            }
            last5_buf[4] = byte;
            if (last5_count < 5) {
                last5_count++;
            }

            // Check if ring buffer is full (head about to overtake tail)
            uint8_t next_head = (rb_head + 1) % RINGBUFFER_SIZE;
            if (next_head == rb_tail) {
                if (!overrun_flag) {
                    overrun_lost_byte = byte;
                    overrun_flag = 1;
                }
            } else {
                rb_buffer[rb_head] = byte;
                rb_head = next_head;
            }
            break;
    }
}

void RingBuffer_Process(void) {
    // 1. Send ":Duksen:" on UART1 when ':' was received on UART0
    if (send_username_flag) {
        send_username_flag = 0;
        char msg[40];
        snprintf(msg, sizeof(msg), ":%s:", USER_NAME);
        SendStringTx(msg);
    }

    // 2. Report buffer overrun as ";missed_number;" on UART1
    if (overrun_flag) {
        uint8_t lost = overrun_lost_byte;
        overrun_flag = 0;

        char err_msg[16];
        snprintf(err_msg, sizeof(err_msg), ";%u;", lost);
        SendStringTx(err_msg);
    }

    // 3. Echo bytes from tail until reaching head
    while (rb_tail != rb_head) {
        uint8_t data = rb_buffer[rb_tail];
        rb_tail = (rb_tail + 1) % RINGBUFFER_SIZE;
        TX_SEND_BYTE(data);
    }
}

void RingBuffer_GetLast5(uint8_t out_bytes[5], uint8_t *count) {
    uint8_t sreg = SREG;
    cli();
    *count = last5_count;
    for (uint8_t i = 0; i < 5; i++) {
        out_bytes[i] = last5_buf[i];
    }
    SREG = sreg;
}

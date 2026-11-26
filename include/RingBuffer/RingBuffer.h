#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include "../ProjectDefines.h"
#include <stdint.h>

#define RINGBUFFER_SIZE 128
#define USER_NAME "Duksen"

void RingBuffer_Init(void);
void RingBuffer_OnRxByte(uint8_t byte);
void RingBuffer_Process(void);
void RingBuffer_GetLast5(uint8_t out_bytes[5], uint8_t *count);
uint8_t RingBuffer_IsRunning(void);


#endif /* RINGBUFFER_H_ */

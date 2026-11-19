#ifndef EXTERNINTERUPTP_H_
#define EXTERNINTERUPTP_H_

#include "../ProjectDefines.h"
#include <stdint.h>

// Initializes External Interrupt 0 (INT0 on PORTD2)
void ExternInteruptP_Init(void);

// Processes button press events safely inside the main loop
void ExternInteruptP_Process(void);

#endif /* EXTERNINTERUPTP_H_ */

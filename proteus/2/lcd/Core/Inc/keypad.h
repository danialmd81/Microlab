#include "stm32f4xx_hal.h"

#define PORTKey GPIOE


void keypad_write(int data);
int keypad_read_pin( int pin);
int keypad_read( void);
char keypad(void);

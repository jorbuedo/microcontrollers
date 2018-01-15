/*
ATmega8, 48, 88, 168, 328

/Reset PC6|1   28|PC5
       PD0|2   27|PC4
       PD1|3   26|PC3 
       PD2|4   25|PC2
       PD3|5   24|PC1
       PD4|6   23|PC0
       Vcc|7   22|Gnd
       Gnd|8   21|Aref
       PB6|9   20|AVcc
       PB7|10  19|PB5 SCK
       PD5|11  18|PB4 MISO
       PD6|12  17|PB3 MOSI / LED
       PD7|13  16|PB2
       PB0|14  15|PB1
*/

#define F_CPU 1000000UL  // 1 MHz standard clock

#include <avr/io.h>     // defines all macros and symbols
#include <util/delay.h> // defines time delay functions 
#include <avr/interrupt.h>

uint8_t i=0;

int main (void)
{
  // see ATmega328 datasheet for TIMER2: phase-correct PWM
  TCCR2A  = (1 << COM2A1) | (0 << COM2A0)
          | (0 << COM2B1) | (0 << COM2B0)
          | (0 << WGM21) | (1 << WGM20);
  TCCR2B  = (0 << WGM22)       
          | (0 << CS22)  | (0 << CS21)  | (1 << CS20);
  OCR2A   = 0;
  DDRB = 0b00001000;     // enable output driver PB3
  while (1)              // infinite main loop
  {
    for (i=0; i<255; i++)
    {
      OCR2A = i;
      _delay_ms(8);
    }
    for (i=255; i>0; i--)
    {
      OCR2A = i;
      _delay_ms(8);
    }
  }
}

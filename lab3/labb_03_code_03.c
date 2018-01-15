/*
ATmega8, 48, 88, 168, 328

/Reset PC6|1   28|PC5
 seg_A PD0|2   27|PC4
 seg_B PD1|3   26|PC3 Anode_4
 seg_C PD2|4   25|PC2 Anode_3
 seg_D PD3|5   24|PC1 Anode_2
 seg_E PD4|6   23|PC0 Anode_1
       Vcc|7   22|Gnd
       Gnd|8   21|Aref
       PB6|9   20|AVcc
       PB7|10  19|PB5 SCK
 seg_F PD5|11  18|PB4 MISO
 seg_G PD6|12  17|PB3 MOSI
 dot   PD7|13  16|PB2
       PB0|14  15|PB1
*/

#define F_CPU 1000000UL  // 1 MHz standard clock

#include <avr/io.h>     // defines all macros and symbols
#include <util/delay.h> // defines time delay functions 
#include <avr/interrupt.h>

uint8_t digit=0;
uint8_t framebuffer[4] = {0b00111111,0b00000110,0b01011011,0b01001111};

ISR(TIMER1_COMPA_vect)          // interrupt service routine
{
  if (digit < 3) {digit++;} else {digit = 0;}
  PORTC &= 0b11110000;          // all digits off
  PORTD = ~framebuffer[digit];  // output bits
  PORTC |= (1 << digit);        // activate digit
}

int main (void)
{
  cli();                 // no interrupts
  DDRD = 0b11111111;     // all pins of PORTD as output
  DDRC = 0b00001111;     // some pins of PORTC as output
  //
  // see ATmega328 datasheet for TIMER1
  //
  TCCR1A = (0 << COM1A1) | (0 << COM1A0)
         | (0 << COM1B1) | (0 << COM1B0)
         | (0 << WGM11)  | (0 << WGM10);
  TCCR1B = (0 << ICNC1)  | (0 << ICES1)
         | (0 << WGM13)  | (1 << WGM12)
         | (1 << CS12)   | (0 << CS11)  | (0 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  OCR1A  = 1952;
  //-----------------------------------
  sei();                 // allow interrupts

  while (1)              // infinite main loop
  {
                         // empty!
  }
}

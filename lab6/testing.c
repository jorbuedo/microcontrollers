/*
ATmega8, 48, 88, 168, 328

/Reset PC6|1   28|PC5
LCD.D4 PD0|2   27|PC4
LCD.D5 PD1|3   26|PC3
LCD.D6 PD2|4   25|PC2
LCD.D7 PD3|5   24|PC1
LCD.RS PD4|6   23|PC0
       Vcc|7   22|Gnd
       Gnd|8   21|Aref
       PB6|9   20|AVcc
       PB7|10  19|PB5 SCK
LCD.RW PD5|11  18|PB4 MISO
LCD.E  PD6|12  17|PB3 MOSI DAC3
       PD7|13  16|PB2      DAC2
DAC0   PB0|14  15|PB1      DAC1
*/

#define F_CPU 1000000UL  // 1 MHz clock

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>


void init (void)         // collect hardware initializations here
{
  DDRC = 0b00001100;
}

int main (void)
{
  float i;
  i = 50;
  init();

  while (1)            // infinite main loop
  {
      PORTC |= 0b00000100;
      i *= 5;
      PORTC &= 0b11111011;
      if(i==0) 
        PORTC |= 0b00001000;
      else
        PORTC &= 0b11110111;

      _delay_us(1);
  }
}

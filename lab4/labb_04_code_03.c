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
LCD.E  PD6|12  17|PB3 MOSI OC2A
       PD7|13  16|PB2      
       PB0|14  15|PB1      
*/

#define F_CPU 1000000UL  // 1 MHz clock

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "lcd.h"

char buffer[20];

static const uint8_t sinetable[] PROGMEM =
{
#include "sinetable_full_8bit_256t.txt"
};

void init (void)         // collect hardware initializations here
{
  lcd_init(LCD_DISP_ON); // initialize LCD
  lcd_puts("8bit sine PWM");

  // see ATmega328 datasheet for TIMER2: 
  //  phase-correct PWM Mode 1
  //  non-inverting
  //  running at f_cpu
  TCCR2A = (1 << COM2A1) | (0 << COM2A0)
         | (0 << WGM21) | (1 << WGM20);
  TCCR2B = (0 << WGM22)
         | (0 << CS22)  | (0 << CS21)  | (1 << CS20);
  OCR2A  = 0;
  DDRB = 0b00001000;     // enable output driver PB3
}

int main (void)
{
  uint8_t i;
  init();

  while (1)            // infinite main loop
  {
    for (i=0; i<256; i++)
    {
      OCR2A = pgm_read_byte(&sinetable[i]);
      _delay_us(78);
    }
  }
}


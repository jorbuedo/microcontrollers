/*
ATmega8, 48, 88, 168, 328

/Reset PC6|1   28|PC5
LCD.D4 PD0|2   27|PC4
LCD.D5 PD1|3   26|PC3
LCD.D6 PD2|4   25|PC2
LCD.D7 PD3|5   24|PC1
LCD.RS PD4|6   23|PC0 ADC0
       Vcc|7   22|Gnd
       Gnd|8   21|Aref
       PB6|9   20|AVcc
       PB7|10  19|PB5 SCK
LCD.RW PD5|11  18|PB4 MISO
LCD.E  PD6|12  17|PB3 MOSI 
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

void init (void)         // collect hardware initializations here
{
  lcd_init(LCD_DISP_ON); // initialize LCD
  lcd_puts("ADC value");

  // ADC init
  //  reference voltage: supply AVCC
  //  channel ADC0
  //  clock: f_cpu/8  (=125kHz)
  //  right-aligned result
  ADMUX = (0 << REFS1) | (1 << REFS0)
        | (0 << ADLAR) 
        | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
  ADCSRA = (1 << ADEN)
         | (1 << ADSC)
         | (0 << ADATE)
         | (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

int main (void)
{
  init();

  while (1)            // infinite main loop
  {
    ADCSRA |= (1 << ADSC);         // start conversion
    while (ADCSRA & (1 << ADSC)) ; // wait for finish
    lcd_gotoxy(10,0);
    lcd_puts("      ");
    lcd_gotoxy(10,0);
    lcd_puts(itoa(ADC, buffer, 10));
    _delay_ms(200);
  }
}


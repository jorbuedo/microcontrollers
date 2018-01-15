/*

screen /dev/tty.usbserial 4800 8N1

ATmega8, 48, 88, 168, 328

    /Reset PC6|1   28|PC5      SCL
RxD        PD0|2   27|PC4      SDA
TxD        PD1|3   26|PC3
           PD2|4   25|PC2
           PD3|5   24|PC1
LCD.D4     PD4|6   23|PC0
           Vcc|7   22|Gnd
           Gnd|8   21|Aref
LCD.RW     PB6|9   20|AVcc
LCD.RS     PB7|10  19|PB5 SCK  
LCD.D5     PD5|11  18|PB4 MISO 
LCD.D6     PD6|12  17|PB3 MOSI 
LCD.D7     PD7|13  16|PB2      
LCD.E      PB0|14  15|PB1      
*/

#define F_CPU 1000000UL  // standard 1 MHz

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "uart.h"

char buffer[20];

void init(void)
{ 
  // initialize LCD
  lcd_init(LCD_DISP_ON); 
  lcd_puts("Testing UART");

  // initialize UART
  uart_init( UART_BAUD_SELECT(4800,F_CPU) ); 
  sei();
}

int main(void)
{
  init();
  while (1)
  {
    uart_puts("asdfgh");
    lcd_puts("asdfgh");
    _delay_ms(100);
  }
  return 0;
}

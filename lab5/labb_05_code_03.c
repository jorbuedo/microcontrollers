#define F_CPU 1000000UL  // 3.68 MHz STK500

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "i2cmaster.h"


//
// define and declare section for DS1307
//
// unsigned 8-bit BCD operations for DS1307
#define fromBCD(x) (((x) >> 4) * 10 + ((x) & 0xF))
#define toBCD(x)   ((((x) / 10) << 4) | ((x) % 10)) 
char *days[7] = {"mån","tis","ons","tor","fre","lör","sön"};
char digits[10] = {'0','1','2','3','4','5','6','7','8','9'};
// internal registers
#define DS1307_ADDR  0xD0
enum {dsSEC, dsMIN, dsHOUR, dsDOW, dsDAY, dsMONTH, dsYEAR, dsSTATUS, DSREGS};
static const uint8_t DS1307_const[DSREGS] PROGMEM = {0x30,0x17,0x13,2,0x26,0x02,0x13,0b00010000};
uint8_t DS1307_regs[DSREGS];

char buffer[20];

//
// Dallas DS1307 RTC
//
void DS1307read(void)
{
  uint8_t i;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 	
  {
    i2c_start_wait(DS1307_ADDR+I2C_WRITE);
    i2c_write(0x00);
    i2c_stop();
    /* Commence the read. */
    _delay_us(20);
    i2c_start_wait(DS1307_ADDR+I2C_READ);
    for (i=0; i<(DSREGS-1); i++)
    {
       _delay_us(10);
       DS1307_regs[i] = i2c_read(1);
    }
    _delay_us(10);
    DS1307_regs[DSREGS-1] = i2c_read(0);
    i2c_stop();
  }
}

void DS1307write(void)
{
  uint8_t i;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 	
  {
    i2c_start_wait(DS1307_ADDR+I2C_WRITE);
    i2c_write(0x00);  //start address
    for (i=0; i<DSREGS; i++)
    {
      i2c_write(DS1307_regs[i]);
    }
    i2c_stop();
  }
}

void init(void)
{ 
  uint8_t i;
  // initialize LCD
  lcd_init(LCD_DISP_ON); 
  lcd_puts("Hej Viktor");

  // initialize RTC
  i2c_init();
  DS1307read();
  if (DS1307_regs[dsSEC] & 0b10000000)
  {
    for (i=0; i<DSREGS; i++)
    {
      DS1307_regs[i] = pgm_read_byte(&DS1307_const[i]);
    }
    DS1307write();
  }
}

int main(void)
{
  uint8_t dummy;
  init();
  while (1)
  {
    DS1307read();
    lcd_clrscr();

    dummy = DS1307_regs[dsHOUR];
    lcd_putc(digits[dummy>>4]);
    lcd_putc(digits[dummy & 0x0f]);
    lcd_putc(':');

    dummy = DS1307_regs[dsMIN];
    lcd_putc(digits[dummy>>4]);
    lcd_putc(digits[dummy & 0x0f]);
    lcd_putc(':');


    dummy = DS1307_regs[dsSEC];
    lcd_putc(digits[dummy>>4]);
    lcd_putc(digits[dummy & 0x0f]);

    lcd_gotoxy(0,1);

    lcd_puts(days[DS1307_regs[dsDOW]-1]);
    lcd_puts(" 20");
    dummy = DS1307_regs[dsYEAR];
    lcd_putc(digits[dummy>>4]);
    lcd_putc(digits[dummy & 0x0f]);
    lcd_putc('-');

    dummy = DS1307_regs[dsMONTH];
    lcd_putc(digits[dummy>>4]);
    lcd_putc(digits[dummy & 0x0f]);
    lcd_putc('-');


    dummy = DS1307_regs[dsDAY];
    lcd_putc(digits[dummy>>4]);
    lcd_putc(digits[dummy & 0x0f]);

    _delay_ms(200);
  }
  return 0;
}

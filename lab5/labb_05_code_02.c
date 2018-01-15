/*
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
LCD.RS     PB7|10  19|PB5 SCK  Nokia.SCL
LCD.D5     PD5|11  18|PB4 MISO Nokia.DC
LCD.D6     PD6|12  17|PB3 MOSI Nokia.SD
LCD.D7     PD7|13  16|PB2      Nokia.SCE
LCD.E      PB0|14  15|PB1      Nokia.RST
*/

#define F_CPU 1000000UL  // standard 1 MHz

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "lcd.h"

//
// macros for the software SPI
//
#define SetSCE     PORTB |= (1 << 2)  // PB2
#define ClearSCE   PORTB &=~(1 << 2)
#define SetRST     PORTB |= (1 << 1)  // PB1
#define ClearRST   PORTB &=~(1 << 1)
#define SetDC      PORTB |= (1 << 4)  // PB4
#define ClearDC    PORTB &=~(1 << 4)
#define SetSD      PORTB |= (1 << 3)  // PB3
#define ClearSD    PORTB &=~(1 << 3)
#define SetSCL     PORTB |= (1 << 5)  // PB5
#define ClearSCL   PORTB &=~(1 << 5)  

#define VOP 0x30         // contrast control setting        
                         //  see marking on LCD module
#define SIZEX 84
#define SIZEY 48

//  6x8 font
//    LSB is top
//    MSB is bottom
//
static const uint8_t smallFont[][6] PROGMEM =
#include "font_6x8_iso8859_1.h"

//
//  define a local copy of the display memory
//
uint8_t framebuffer[SIZEX*SIZEY/8];

char buffer[20];

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeCommand
  Description  :  Sends command to display controller.
  Argument(s)  :  command -> command to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeCommand (uint8_t command )
{
    uint8_t i;
    ClearSCE;       //enable LCD
    ClearDC;        // set LCD into command mode
    ClearSCL;
    for (i=0; i<8; i++)
    {
      if (command & 0b10000000)
      {
        SetSD;
      }
      else
      {
        ClearSD;
      }
//    _delay_us(1);
      SetSCL;       // minimum 100 ns
//    _delay_us(1);
      ClearSCL;     // minimum 100 ns
      command <<= 1;
    }
    SetSCE;         // disable LCD
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeData
  Description  :  Sends data to display controller.
  Argument(s)  :  data -> data to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeData (uint8_t data )
{
    uint8_t i;
    ClearSCE;       // enable LCD
    SetDC;          // set LCD in data mode
    ClearSCL;
    for (i=0; i<8; i++)
    {
      if (data & 0b10000000)
      {
        SetSD;
      }
      else
      {
        ClearSD;
      }
//    _delay_us(1);
      SetSCL;       // minimum 100 ns
//    _delay_us(1);
      ClearSCL;     // minimum 100 ns
      data <<= 1;
    }
    SetSCE;         // disable LCD
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_gotoXY
  Description  :  Sets cursor location to xy location corresponding to basic font size.
  Argument(s)  :  x - range: 0 to 84
                  y -> range: 0 to 5
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_gotoXY ( uint8_t x, uint8_t y )
{
    LCD_writeCommand (0x80 | x);   //column
    LCD_writeCommand (0x40 | y);   //row
}

void LCD_clearbuffer(void)
{
  memset(framebuffer, 0x00, SIZEX*SIZEY/8);
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_update
  Description  :  transfers the local copy to the display
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_update (void)
{
  uint16_t i;

  LCD_gotoXY(0,0);      // start with (0,0) position
  for(i=0; i<(SIZEX*SIZEY/8); i++)
  {
    LCD_writeData(framebuffer[i]);
  }
  LCD_gotoXY(0,0);      // bring the XY position back to (0,0)
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_clear
  Description  :  Clears the display
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_clear ( void )
{
    LCD_clearbuffer();
    LCD_update();
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_init
  Description  :  LCD controller initialization.
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_init ( void )
{
    DDRB |= 0b00111110;
    _delay_ms(100);

    ClearSCE;                  // Enable LCD

    ClearRST;                  // reset LCD
    _delay_ms(100);
    SetRST;

    SetSCE;                    //disable LCD

    LCD_writeCommand( 0x21 );  // LCD Extended Commands.
    LCD_writeCommand( 0x80 | VOP );  // Set LCD Vop (Contrast).
    LCD_writeCommand( 0x04 );  // Set Temp coefficent.
    LCD_writeCommand( 0x13 );  // LCD bias mode 1:48.
    LCD_writeCommand( 0x20 );  // LCD Standard Commands, Horizontal addressing mode.
    LCD_writeCommand( 0x0c );  // LCD in normal mode.

    LCD_clear();
}

void LCD_setVop(uint8_t vop)
{
    LCD_writeCommand( 0x21 );  // LCD Extended Commands.
    LCD_writeCommand( 0x80 | vop );  // Set LCD Vop (Contrast).
    LCD_writeCommand( 0x20 );  // LCD Standard Commands, Horizontal addressing mode.
    LCD_writeCommand( 0x0c );  // LCD in normal mode.
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_setpixel
  Description  :  Sets the pixel at xy location
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_setpixel(uint8_t x, uint8_t y)
{
  if ((x < SIZEX) && (y < SIZEY))
  {
    framebuffer[(uint16_t) x+SIZEX*(y/8)] |= (1 << (y % 8));
  }
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_clearpixel
  Description  :  Clears the pixel at xy location
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_clearpixel(uint8_t x, uint8_t y)
{
  if ((x < SIZEX) && (y < SIZEY))
  {
    framebuffer[(uint16_t) x+SIZEX*(y/8)] &= ~(1 << (y % 8));
  }
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_putchar
  Description  :  puts a single character onto LCD
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
                  ch - character
                  attr - attribute 0-normal, 1-inverse, 2-underline
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_putchar(uint8_t x0, uint8_t y0, char ch, uint8_t attr)
{
  uint8_t yd, ym, i, fontbyte;
  uint16_t m;
  yd = y0/8;
  ym = y0%8;
  for (i=0; i<6; i++)
  {
    fontbyte = pgm_read_byte(&smallFont[(uint8_t)ch][i]);
    switch (attr)
    {
      case  0:
          break;
      case  1: 
          fontbyte ^= 0xff;
          break;
      case  2: 
          fontbyte |= 0b10000000;
          break;
    }

    if ((x0+i)<SIZEX)
    {
      m = (uint16_t) x0+i+SIZEX*(yd);
      framebuffer[m] &= ~(0xff << ym);
      framebuffer[m] |= (fontbyte << ym);
      if ((y0<(SIZEY-8)) && (ym != 0))
      {
        m = (uint16_t) x0+i+SIZEX*(yd+1);
        framebuffer[m] &= ~(0xff >> (8-ym));
        framebuffer[m] |= (fontbyte >> (8-ym));
      }
    }
  }
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_print
  Description  :  prints a string
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
                  *ch - pointer t string
                  attr - attribute 0-normal, 1-inverse, 2-underline
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_print(uint8_t x, uint8_t y, char *ch,uint8_t attr)
{
  while (*ch)
  {
    LCD_putchar(x, y, *ch,attr);
    ch++;
    x += 6;
  }
}

void init(void)
{ 
  // initialize LCD
  lcd_init(LCD_DISP_ON); 
  lcd_puts("Testing 5110");

  // initialize Nokia5110
  LCD_init();
  LCD_clear();
  _delay_ms(20);
}

int main(void)
{
  uint8_t i=0, x, y;
  init();
  while (1)
  {
    for (y=0; y<6; y++)
    {
      for (x=0; x<14; x++)
      {
        LCD_putchar(x*6, y*8, i, 0);
        LCD_update();
        lcd_gotoxy(0,1);
        lcd_puts("   ");
        lcd_gotoxy(0,1);
        lcd_puts(itoa(i,buffer,10));
        i++;
      }
    }
    _delay_ms(500);
    LCD_clear();
  }
  return 0;
}

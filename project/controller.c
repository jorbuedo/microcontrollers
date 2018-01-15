/*

screen /dev/tty.usbserial 4800 8N1

ATmega8, 48, 88, 168, 328

	/Reset 	PC6|1   28|PC5      SCL
RxD        	PD0|2   27|PC4      SDA
TxD        	PD1|3   26|PC3     
		   	PD2|4   25|PC2     
		   	PD3|5   24|PC1     
		    PD4|6   23|PC0
		   	Vcc|7   22|Gnd
		   	Gnd|8   21|Aref
		    PB6|9   20|AVcc
		    PB7|10  19|PB5 SCK  
		    PD5|11  18|PB4 MISO 
		    PD6|12  17|PB3 MOSI 
		    PD7|13  16|PB2      
		    PB0|14  15|PB1      
*/

#define F_CPU 1000000UL  // standard 1 MHz

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "uart.h"

#include "lcd.h"
#include "mpu6050.h"
#include "base64.h"
//#include "nrf24l01.h"

#define UART_RX_BUFFER_SIZE = 64
#define UART_TX_BUFFER_SIZE = 64

typedef struct controller_datagram{
	uint16_t buttons;
	uint16_t adcAX;
	uint16_t adcAY;
	uint16_t adcBX;
	uint16_t adcBY;
	double roll;
	double pitch;
	double yaw;
}controller_datagram;

volatile controller_datagram data;

char buffer[20];

// ADC interrupt
ISR(ADC_vect) {
	uint8_t mux_count;
	mux_count = ADMUX;
	mux_count &= 0x0F;
	switch(mux_count){
		case 0: data.adcAX = ADCW; break;
		case 1: data.adcAY = ADCW; break;
		case 2: data.adcBX = ADCW; break;
		case 3: data.adcBY = ADCW; break;
		default: break;
	}
}

volatile uint8_t aux = 0;
ISR(PCINT0_vect){
	if(aux) aux--;
	else aux=2;
	data.buttons = PINB;
}


void init(void) {
	// init change interrupt PB0
	DDRB &=  0b11110000;
	DDRB |=  0b11110110;
	DDRD |=  0b10000000;
	PORTB |= 0b00001111;
	PCICR |= (0 << PCIE2) | (0 << PCIE1) | (1 << PCIE0);
	PCMSK0 |= (1 << PCINT0);

	
	// initialize LCD
	lcd_init(LCD_DISP_ON);


	// init accelerometer
	mpu6050_init();
	_delay_ms(50);
	mpu6050_dmpInitialize();
	mpu6050_dmpEnable();


	// initialize UART
//	uart_init( UART_BAUD_SELECT(2400,F_CPU) ); 


	// ADC init
	ADMUX = (0 << REFS1) | (1 << REFS0)
			| (0 << ADLAR) // Left aligned
			| (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
	ADCSRA = (1 << ADEN)
			| (1 << ADSC)
			| (0 << ADATE) | (1 << ADIE)
			| (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);


	//enable interrupts
	sei();


	data.buttons = 0;
	data.adcAX=500;
	data.adcAY=500;
	data.adcBX=500;
	data.adcBY=500;
	data.roll = 0.0;
	data.pitch = 0.0;
	data.yaw = 0.0;


}

int main(void) {
	//Accelerometer variables
	double qw = 1.0f;
	double qx = 0.0f;
	double qy = 0.0f;
	double qz = 0.0f;
	double roll = 0.0f;
	double pitch = 0.0f;
	double yaw = 0.0f;

	char b64str[Base64encode_len(sizeof(data))];

	init();

	while (1) {
		// Accelerometer
		if(mpu6050_getQuaternionWait(&qw, &qx, &qy, &qz)) {
			mpu6050_getRollPitchYaw(qw, qx, qy, qz, &roll, &pitch, &yaw);
		}
		_delay_ms(10);

		data.roll = roll;
		data.pitch = pitch;
		data.yaw = yaw;

		// Potentiometers
		ADMUX &= 0xF0;
		ADMUX |= (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC));
	
		ADMUX &= 0xF0;
		ADMUX |= (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (1 << MUX0);
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC));

		ADMUX &= 0xF0;
		ADMUX |= (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0);
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC));

		ADMUX &= 0xF0;
		ADMUX |= (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0);
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC));


if (data.adcAX>600 && data.adcAY <400)
PORTB ^= _BV(PB1);
else if(data.adcAY>600 && data.adcAX <400)
PORTD ^= _BV(PD7);
else if(data.adcAX>600 && data.adcAY >600)
PORTB ^= _BV(PB2);
else if(data.adcAX<400 && data.adcAY <400)
PORTB ^= _BV(PB7);
else {PORTB &= 0b01111001;
PORTD &= 0b01111111;
}




		char itmp[10];
if(aux){
		lcd_clrscr();
				lcd_gotoxy(0,0);

		lcd_puts("X=");
lcd_gotoxy(8,0);
lcd_puts("Y=");
lcd_gotoxy(0,1);
lcd_puts("Z=");
		lcd_gotoxy(2,0);
lcd_puts(dtostrf(roll, 3, 2, buffer));
lcd_gotoxy(10,0);
lcd_puts(dtostrf(pitch, 3, 2, buffer));
lcd_gotoxy(2,1);
lcd_puts(dtostrf(yaw, 3, 2, buffer));
/*		lcd_gotoxy(0,0);
		dtostrf(roll, 3, 2, itmp);	lcd_puts(itmp);

		lcd_gotoxy(0,1);
		dtostrf(pitch, 3, 2, itmp);	lcd_puts(itmp);

		lcd_gotoxy(9,1);
		dtostrf(yaw, 3, 2, itmp);	lcd_puts(itmp);
*/
} else {


		//Send stuff
//		Base64encode(b64str, &data, sizeof(data));
//		uart_puts(b64str);

/*		PORTB |= (0x7);
		PORTD |= (0x80);

		if(data.adcAX>800 && data.adcAY>800){
			PORTB |= 0x2;
		}
		if(data.adcAX<200 && data.adcAY<200){
			PORTD |= 0x80;
		}
		if(data.adcAX>800 && data.adcAY<200){
			PORTB |= 0x1;
		}
		if(data.adcAX<200 && data.adcAY>800){
			PORTB |= 0x4;
		}
*/
	lcd_clrscr();


	lcd_puts("AX");
	lcd_gotoxy(0,1);
	lcd_puts("AY");
	lcd_gotoxy(8,0);
	lcd_puts("BX");
	lcd_gotoxy(8,1);
	lcd_puts("BY");
	lcd_gotoxy(3,0);
	lcd_puts("     ");
	lcd_gotoxy(3,0);
	lcd_puts(itoa(data.adcBX, buffer, 10));

	lcd_gotoxy(3,1);
	lcd_puts("     ");
	lcd_gotoxy(3,1);
	lcd_puts(itoa(data.adcBY, buffer, 10));

	lcd_gotoxy(11,0);
	lcd_puts("     ");
	lcd_gotoxy(11,0);
	lcd_puts(itoa(data.adcAY, buffer, 10));

	lcd_gotoxy(11,1);
	lcd_puts("     ");
	lcd_gotoxy(11,1);
	lcd_puts(itoa(data.adcAX, buffer, 10));

}
//		data.adcBX=200;
//		data.adcBY=200;

	}
	return 0;
}



/*

screen /dev/tty.usbserial 4800 8N1

ATmega8, 48, 88, 168, 328

	/Reset 	PC6|1   28|PC5      SCL
RxD        	PD0|2   27|PC4      SDA
TxD        	PD1|3   26|PC3     
		   	PD2|4   25|PC2     
R		   	PD3|5   24|PC1     
		    PD4|6   23|PC0
		   	Vcc|7   22|Gnd
		   	Gnd|8   21|Aref
		    PB6|9   20|AVcc
		    PB7|10  19|PB5 SCK  
G		    PD5|11  18|PB4 MISO 
B		    PD6|12  17|PB3 MOSI 
		    PD7|13  16|PB2      
		    PB0|14  15|PB1      
*/

#define F_CPU 1000000UL  // standard 1 MHz

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"


#include "lcd.h"
#include "base64.h"

#define UART_RX_BUFFER_SIZE = 64
#define UART_TX_BUFFER_SIZE = 64

#define RED_LED		OCR2B
#define GREEN_LED	OCR0B
#define BLUE_LED	OCR0A

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

volatile char dotAX=0, dotAY=2, dotBX=4, dotBY=5;
volatile uint16_t dot=0;


ISR(TIMER0_OVF_vect) {
	dot++;
	if(dot%2==0){
		if(dot%10000==0){
			if (data.adcAX < 400) {
				dotAX = (!dotAX)? 4 : dotAX -1; 
			} else if(data.adcAX > 700) {
				dotAX++;
				dotAX%=5;
			}
			if (data.adcAY < 400) {
				dotAY = (!dotAY)? 7 : dotAY -1; 
			} else if(data.adcAY > 700) {
				dotAY++;
				dotAY%=8;
			}
		}
		PORTC = 0x01;
		PORTB = 0xFF;

		PORTC |= (1<<(dotAX+1));
		PORTB ^= (1<<dotAY);
	} else {
		if(dot%5000 == 1){
			if (data.adcBX < 400) {
				dotBX = (!dotBX)? 4 : dotBX -1; 
			} else if(data.adcBX > 700) {
				dotBX++;
				dotBX%=5;
			}
			if (data.adcBY < 400) {
				dotBY = (!dotBY)? 7 : dotBY -1; 
			} else if(data.adcBY > 700) {
				dotBY++;
				dotBY%=8;
			}
		}
		PORTC = 0x01;
		PORTB = 0xFF;

		PORTC |= (1<<(dotBX+1));
		PORTB ^= (1<<dotBY);
	}
}

void init(void) {
	// initialize rgb output
	DDRD = 0b01101000;

	// Matrix
	DDRC = 0x3E;
	DDRB = 0xFF;

	//Set timer
	TIMSK0=(1<<TOIE0);
	TCNT0=0x00;
	TCCR0B = (1<<CS01) | (1<<CS00);

	// initialize UART
	uart_init( UART_BAUD_SELECT(2400,F_CPU) ); 


	// initializr PWM 0A, 0B, 2B
	TCCR0A = (1 << COM0A1) | (0 << COM0A0) | (1 << COM0B1)
			| (0 << COM0B0) | (0 << WGM01) | (1 << WGM00);
	TCCR0B = (0 << WGM02) | (0 << CS02)  | (0 << CS01)  | (1 << CS00);
	OCR0A  = 0;
	OCR0B  = 0;

	TCCR2A = (1 << COM2B1) | (0 << COM2B0) | (0 << WGM21) | (1 << WGM20);
	TCCR2B = (0 << WGM22) | (0 << CS22)  | (0 << CS21)  | (1 << CS20);
	OCR2B  = 0;

	//enable interrupts
	sei();

	data.buttons = 0;
	data.adcAX=800;
	data.adcAY=800;
	data.adcBX=200;
	data.adcBY=200;
	data.roll = 0.0;
	data.pitch = 0.0;
	data.yaw = 0.0;

	RED_LED = 10;
	GREEN_LED = 10;
	BLUE_LED = 10;
}


int main(void) {
	init();

	int c, i=0;
	char b64str[Base64encode_len(sizeof(data))];


	while (1) {
/*		do{
			do{
				c = uart_getc();
			} while((c & UART_NO_DATA));
			b64str[i++] = (char)c;
		} while((char)c != '\0');

		Base64decode(&data, b64str);
		i=0;
*/
		RED_LED++;
		GREEN_LED++;
		BLUE_LED++;
		_delay_ms(10);

	}

	return 0;
}






















/*







#define F_CPU 1000000UL  // standard 1 MHz

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"


#define RED_LED		OCR2B
#define GREEN_LED	OCR0B
#define BLUE_LED	OCR0A


void init(void) {
	// initialize rgb output
	DDRD = 0b01101000;


	// initialize UART
	uart_init( UART_BAUD_SELECT(9600,F_CPU) ); 


	// initializr PWM 0A, 0B, 2B
	TCCR0A = (1 << COM0A1) | (0 << COM0A0) | (1 << COM0B1)
			| (0 << COM0B0) | (0 << WGM01) | (1 << WGM00);
	TCCR0B = (0 << WGM02) | (0 << CS02)  | (0 << CS01)  | (1 << CS00);
	OCR0A  = 0;
	OCR0B  = 0;
	TCCR2A = (1 << COM2B1) | (0 << COM2B0) | (0 << WGM21) | (1 << WGM20);
	TCCR2B = (0 << WGM22) | (0 << CS22)  | (0 << CS21)  | (1 << CS20);
	OCR2B  = 0;

	//enable interrupts
	sei();

	RED_LED = 10;
	GREEN_LED = 10;
	BLUE_LED = 10;
}


int main(void) {
	init();

	int c, i=0;


	while (1) {
		do{
			c = uart_getc();
		} while((c & UART_NO_DATA));
		if((c&0x00FF) == 'G') {
			RED_LED = 0;
			GREEN_LED = 70;
			BLUE_LED = 0;
		}

/*		do{
			do{
				c = uart_getc();
			} while((c & UART_NO_DATA));
			b64str[i++] = (char)c;
		} while((char)c != '\0');

	}

	return 0;
}
*/
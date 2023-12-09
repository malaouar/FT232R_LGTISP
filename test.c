#include "avr/io.h"
#include <util/delay.h>

#define BAUD   9600  
#include <util/setbaud.h>

//===================================================
void uart_init(void ){
	// Set baud rate
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0)); // default cleared
#endif
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);  //  Enable receiver and transmitter
	//UCSR0C = (1<<URSEL)|(3<<UCSZ0);// default 8N1 
}

//===================================
void sendchar(char c){
	while ( !( UCSR0A & 1<<UDRE0)); // Wait for empty transmit buffer
	UDR0 = c;	// Put data into buffer  
}

//===================================================
void sendstr(char *str){
	while(*str) sendchar(*str++);
}

//====================================================================
// convert a byte to HEX representation
void b2h(uint8_t byte){  
	uint8_t y;
   
	static char const hexchars[16] = "0123456789ABCDEF" ; 
	y=((byte&0xF0)>>4);  // get high nible
	y=hexchars[y % 16];
	sendchar(y);
	y=(byte&0x0F);  // get low nible
	y=hexchars[y % 16];
	sendchar(y);
	sendchar(' ');
}

//===================================================
void main(){	  
	// default clock source is the internal 32MHz and prescaler value is divide by 8 so clk = 32/8 = 4Mhz
	// To run @32MHz change prescaler value to divide by 1	
	CLKPR = 0x80;		// CWE (bit7)= 1 (Enable change)
	CLKPR = 0x20;		// CKOEN0 (bit5) = 1 => system clock is output from PBO pin
						// PS[3:0] (bits3:0)= 0000 => division factor = 1
	
	uart_init();
	sendstr("!!! Hello FTDI FT232R !!! \r\n");
	
	// print PMCR and CLKPR values
	sendstr("PMCR = ");
	b2h(PMCR);
	sendstr("\r\n");
	sendstr("CLKPR = ");	
	b2h(CLKPR);
	sendstr("\r\n");
	
	// print GUID value
	sendstr("GUID = ");
	b2h(GUID0); b2h(GUID1); b2h(GUID2); b2h(GUID3);
	sendstr("\r\n");
	
	// blink
	DDRB = 1<<5;		// PB5 output
	for(;;){
		asm volatile("sbi 0x03,5;"); //PINB = 0x03
		_delay_ms(1000);
	}
}

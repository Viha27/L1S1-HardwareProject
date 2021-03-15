 /*
ds18b20 lib 0x02

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "ds18b20A.h"

/*
 * ds18b20 init
 */

uint8_t ds18b20A_reset() {
	uint8_t i;

	//low for 480us
	DS18B20A_PORT &= ~ (1<<DS18B20A_DQ); //low
	DS18B20A_DDR |= (1<<DS18B20A_DQ); //output
	_delay_us(480);

	//release line and wait for 60uS
	DS18B20A_DDR &= ~(1<<DS18B20A_DQ); //input
	_delay_us(60);

	//get value and wait 420us
	i = (DS18B20A_PIN & (1<<DS18B20A_DQ));
	_delay_us(420);

	//return the read value, 0=ok, 1=error
	return i;
}

/*
 * write one bit
 */
void ds18b20A_writebit(uint8_t bit){
	//low for 1uS
	DS18B20A_PORT &= ~ (1<<DS18B20A_DQ); //low
	DS18B20A_DDR |= (1<<DS18B20A_DQ); //output
	_delay_us(1);

	//if we want to write 1, release the line (if not will keep low)
	if(bit)
		DS18B20A_DDR &= ~(1<<DS18B20A_DQ); //input

	//wait 60uS and release the line
	_delay_us(60);
	DS18B20A_DDR &= ~(1<<DS18B20A_DQ); //input
}

/*
 * read one bit
 */
uint8_t ds18b20A_readbit(void){
	uint8_t bit=0;

	//low for 1uS
	DS18B20A_PORT &= ~ (1<<DS18B20A_DQ); //low
	DS18B20A_DDR |= (1<<DS18B20A_DQ); //output
	_delay_us(1);

	//release line and wait for 14uS
	DS18B20A_DDR &= ~(1<<DS18B20A_DQ); //input
	_delay_us(14);

	//read the value
	if(DS18B20A_PIN & (1<<DS18B20A_DQ))
		bit=1;

	//wait 45uS and return read value
	_delay_us(45);
	return bit;
}

/*
 * write one byte
 */
void ds18b20A_writebyte(uint8_t byte){
	uint8_t i=8;
	while(i--){
		ds18b20A_writebit(byte&1);
		byte >>= 1;
	}
}

/*
 * read one byte
 */
uint8_t ds18b20A_readbyte(void){
	uint8_t i=8, n=0;
	while(i--){
		n >>= 1;
		n |= (ds18b20A_readbit()<<7);
	}
	return n;
}

/*
 * get temperature
 */
double ds18b20A_gettemp() {
	uint8_t temperature_l;
	uint8_t temperature_h;
	double retd = 0;

	#if DS18B20A_STOPINTERRUPTONREAD == 1
	cli();
	#endif

	ds18b20A_reset(); //reset
	ds18b20A_writebyte(DS18B20A_CMD_SKIPROM); //skip ROM
	ds18b20A_writebyte(DS18B20A_CMD_CONVERTTEMP); //start temperature conversion

	while(!ds18b20A_readbit()); //wait until conversion is complete

	ds18b20A_reset(); //reset
	ds18b20A_writebyte(DS18B20A_CMD_SKIPROM); //skip ROM
	ds18b20A_writebyte(DS18B20A_CMD_RSCRATCHPAD); //read scratchpad

	//read 2 byte from scratchpad
	temperature_l = ds18b20A_readbyte();
	temperature_h = ds18b20A_readbyte();

	#if DS18B20A_STOPINTERRUPTONREAD == 1
	sei();
	#endif

	//convert the 12 bit value obtained
	retd = ( ( temperature_h << 8 ) + temperature_l ) * 0.0625;

	return retd;
}


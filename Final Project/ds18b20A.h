/*
ds18b20 lib 0x02

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.

References:
  + Using DS18B20 digital temperature sensor on AVR microcontrollers
    by Gerard Marull Paretas, 2007
    http://teslabs.com/openplayer/docs/docs/other/ds18b20_pre1.pdf
*/


#ifndef DS18B20A_H_
#define DS18B20A_H_

#include <avr/io.h>

//setup connection
#define DS18B20A_PORT PORTA
#define DS18B20A_DDR DDRA
#define DS18B20A_PIN PINA
#define DS18B20A_DQ PA0

//commands
#define DS18B20A_CMD_CONVERTTEMP 0x44
#define DS18B20A_CMD_RSCRATCHPAD 0xbe
#define DS18B20A_CMD_WSCRATCHPAD 0x4e
#define DS18B20A_CMD_CPYSCRATCHPAD 0x48
#define DS18B20A_CMD_RECEEPROM 0xb8
#define DS18B20A_CMD_RPWRSUPPLY 0xb4
#define DS18B20A_CMD_SEARCHROM 0xf0
#define DS18B20A_CMD_READROM 0x33
#define DS18B20A_CMD_MATCHROM 0x55
#define DS18B20A_CMD_SKIPROM 0xcc
#define DS18B20A_CMD_ALARMSEARCH 0xec

//stop any interrupt on read
#define DS18B20A_STOPINTERRUPTONREAD 1

//functions
extern double ds18b20A_gettemp();

#endif

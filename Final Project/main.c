/*
 * Final Project.c
 *
 * Created: 7/31/2019 10:06:11 PM
 * Author : Amesh M Jayaweera
 */ 


#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"
#include "ds18b20.h"
#include "ds18b20A.h"
#define UART_BAUD_RATE 2400
#include "uart.h"

char numberpressed();
void delay(int n);
int delayForMotor(int n,double temp);
void startFan();
void stopFan();
void startMotor();
void stopMotor();
void startValveA();
void stopValveA();
void startValveB();
void stopValveB();


int main(void)
{
	DDRD = 0xFF; // D4-D7 for LCD data pins.
	DDRC = 0xFE; // C0 for Temperature sensor A
	/*	A0 for Temperature sensor B
		A1 for FAN
		A2 for Motor
		A3 for Valve A
		A4 for Valve B */
	DDRA = 0xFE; 
	DDRB = 0xF0; // PORTB for keypad
	
    char printbuff1[100],printbuff2[100],num[100];
	double d1,d2,temp;
	char c;
	int i,n,res;
	
	stopMotor();
	stopValveA();
	stopValveB();
	stopFan();

	Lcd4_Init();
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String(" Welcome ");
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String(" Group No. 19 ");
	delay(3);
	
    while (1) 
    {	
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Enter temp ");
	
		i = 0;
		while(1){
			
			c = numberpressed();
			if(c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9' || c == '0'){
				Lcd4_Set_Cursor(2,i);
				Lcd4_Write_Char(c);
				num[i] = c;
				++i;
				_delay_ms(500);
			}
			
			if(c == 'b'){
				Lcd4_Clear();
				Lcd4_Set_Cursor(1,0);
				Lcd4_Write_String("Running ... ");
				n = atoi(num);
				temp = (double) n;
				while(1){
					d1 = ds18b20_gettemp();
					Lcd4_Set_Cursor(2,0);
					Lcd4_Write_String("Temp: ");
					dtostrf(d1, 10, 3, printbuff1);
					Lcd4_Write_String(printbuff1);
					_delay_ms(20);
					if(d1 >= 36.0){  // Replace with 80
						Lcd4_Clear();
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("Start Valve A");
						startValveA();
						delay(10);  // set the time
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("Stop Valve A");
						stopValveA();
						_delay_ms(20);
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("Start Valve B");
						startValveB();
						delay(10); // set the time
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("Stop Valve A");
						stopValveB();
						_delay_ms(20);
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("Start Fan");
						Lcd4_Set_Cursor(2,0);
						Lcd4_Write_String("Start Motor");
						startMotor();
						startFan();
						res = delayForMotor(10, temp); // set the time
						Lcd4_Set_Cursor(2,0);
						Lcd4_Write_String("Stop Motor");
						stopMotor();
						if(res == 0){
							while(1){
								d2 = ds18b20A_gettemp();
								Lcd4_Set_Cursor(1,0);
								Lcd4_Write_String("Temp: ");
								dtostrf(d2, 10, 3, printbuff2);
								Lcd4_Write_String(printbuff2);
								if(d2 <= temp){
									stopFan();
									Lcd4_Set_Cursor(1,0);
									Lcd4_Write_String("Stop Fan");
									delay(2);
									break;
								}
							}
						}
						_delay_ms(20);
						break;
					}
					_delay_ms(20);	
				}
				break;
			}
		}
    }
}

int delayForMotor(int n,double temp){
	int i,flag;
	double d;
	flag = 0;
	char printbuff[100];
	for(i=0;i<n;i++){
		d = ds18b20A_gettemp();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Temp: ");
		dtostrf(d, 10, 3, printbuff);
		Lcd4_Write_String(printbuff);
		if(d <= temp ){
			stopFan();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("Stop Fan");
			flag = 1;
		}
	}
	return flag;
}

void startValveA(){
	PORTA &= ~(1<<PA3);
}

void stopValveA(){
	PORTA |= (1<<PA3); 
}

void startValveB(){
	PORTA &= ~(1<<PA4);
}

void stopValveB(){
	PORTA |= (1<<PA4); 
}

void startMotor(){
	PORTA &= ~(1<<PA2);
}

void stopMotor(){
	PORTA |= (1<<PA2); 
}

void startFan(){
	PORTA &= ~(1<<PA1);
}

void stopFan(){
	PORTA |= (1<<PA1); 
}

void delay(int n){
	int i;
	for(i=0;i<n;i++)
		_delay_ms(1000);
}


char numberpressed()
{
	PORTB = 0b10000000;
	if(PINB & (1<<PB0))
	{
		return 'a';
	}
	if(PINB & (1<<PB1))
	{
		return '3';
	}
	if(PINB & (1<<PB2))
	{
		return '2';
	}
	if(PINB & (1<<PB3))
	{
		return '1';
	}
	
	PORTB = 0b01000000;
	if(PINB & (1<<PB0))
	{
		return 'b';
	}
	if(PINB & (1<<PB1))
	{
		return '6';
	}
	if(PINB & (1<<PB2))
	{
		return '5';
	}
	if(PINB & (1<<PB3))
	{
		return '4';
	}
	PORTB = 0b00100000;
	if(PINB & (1<<PB0))
	{
		return 'c';
	}
	if(PINB & (1<<PB1))
	{
		return '9';
	}
	if(PINB & (1<<PB2))
	{
		return '8';
	}
	if(PINB & (1<<PB3))
	{
		return '7';
	}
	PORTB = 0b00010000;
	if(PINB & (1<<PB0))
	{
		return 'd';
	}
	if(PINB & (1<<PB1))
	{
		return '#';
	}
	if(PINB & (1<<PB2))
	{
		return '0';
	}
	if(PINB & (1<<PB3))
	{
		return '*';
	}
}

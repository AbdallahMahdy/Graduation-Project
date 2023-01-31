#define F_CPU 8000000UL
#define _NOP()  asm("nop")
#define SREG    _SFR_IO8(0x3f)
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "lcd.h"
#include "utils.h"
#include "spi.h"
#include "mfrc522.h"
#include "I2c.h"
#include "eeprom.h"
#include "std_types.h"
#include "gsm.h"
#include "wifi.h"
#include "gps.h"

#define gsm 	1
#define gps 	2
#define wifi 	3

int active=1;

extern int8_t Response_Status, CRLF_COUNT ;
extern uint16_t Counter;
 extern uint32_t TimeOut;
extern char RESPONSE_BUFFER[DEFAULT_BUFFER_SIZE];
extern int8_t Response_Status1;
extern volatile int16_t Counter1, pointer;
extern uint32_t TimeOut1;
extern char RESPONSE_BUFFER1[DEFAULT_BUFFER_SIZE1];

extern char Latitude_Buffer[15],Longitude_Buffer[15],Time_Buffer[15],Altitude_Buffer[8];
extern char degrees_buffer[degrees_buffer_size]; /* save latitude or longitude in degree */
extern char GGA_Buffer[Buffer_Size]; /* save GGA string */
extern uint8_t GGA_Pointers[20]; /* to store instances of ',' */
extern char GGA_CODE[3];
extern volatile uint16_t GGA_Index, CommaCounter;

extern bool IsItGGAString ,flag1,flag2 ;

uint8_t SelfTestBuffer[64];

void activate(int n)
{
	if(n==1){
		USART_Init(19200);
		DIOSetPinValue(3,6,0);
		DIOSetPinValue(3,7,0);
		active=1;
	}
	if(n==2){
			USART_Init(9600);
			DIOSetPinValue(3,6,1);
			DIOSetPinValue(3,7,0);
			active=2;
		}
	if(n==3){
			USART_Init(115200);
			DIOSetPinValue(3,6,0);
			DIOSetPinValue(3,7,1);
			active=3;
		}
}

int main()
{
uint32_t Check=0;
GGA_Index=0;
DIOSetPinDirection(1,0,1);
DIOSetPinDirection(1,1,1);
DIOSetPinDirection(1,2,1);
DIOSetPinDirection(1,2,1);
DIOSetPinDirection(3,6,1);
DIOSetPinDirection(3,7,1);
uint8_t data;
uint8_t byte;
uint8_t str[MAX_LEN];
LCDWriteStringXY(2,0,"initializing RFID");
_delay_ms(350);
LCDClear();
LCDInit(LS_BLINK);
LCDWriteStringXY(2,0,"RFID Reader");
LCDWriteStringXY(5,1,VERSION_STR);


spi_init();
_delay_ms(1000);
LCDClear();

//init reader
mfrc522_init();

//check version of the reader
byte = mfrc522_read(VersionReg);
if(byte == 0x92)
{
	LCDWriteStringXY(2,0,"MIFARE RC522v2");
	LCDWriteStringXY(4,1,"Detected");
}else if(byte == 0x91 || byte==0x90)
{
	LCDWriteStringXY(2,0,"MIFARE RC522v1");
	LCDWriteStringXY(4,1,"Detected");
}else
{
	LCDWriteStringXY(0,0,"No reader found");
}

byte = mfrc522_read(ComIEnReg);
mfrc522_write(ComIEnReg,byte|0x20);
byte = mfrc522_read(DivIEnReg);
mfrc522_write(DivIEnReg,byte|0x80);

_delay_ms(1500);
LCDClear();
LCDWriteStringXY(2,0,"Initializing memory");
_delay_ms(300);
EEpromInit();
LCDClear();
activate(gsm);
LCDWriteStringXY(2,0,"Initializing GSM");


char _buffer[150];
	USART_Init(19200);							/* Initiate USART with 19200 baud rate */
	sei();										/* Start global interrupt */
	while(!SIM900HTTP_Start());
	while(!(SIM900HTTP_Connect(APN, USERNAME, PASSWORD)));
	SIM900HTTP_Init();
	_delay_ms(300);
	LCDClear();
	activate(gps);
	LCDWriteStringXY(2,0,"Initializing GPS");

		memset(GGA_Buffer, 0, Buffer_Size);
		memset(degrees_buffer,0,degrees_buffer_size);

	    _delay_ms(3000);                 /* wait for GPS receiver to initialize */
	    USART_Init(9600);                /* initialize USART with 9600 baud rate */
		sei();
	_delay_ms(300);
	activate(wifi);
	LCDClear();
	LCDWriteStringXY(2,0,"Initializing wifi");
	uint8_t Connect_Status;

		USART_Init(115200);						/* Initiate USART with 115200 baud rate */
		sei();									/* Start global interrupt */

		while(!ESP8266_Begin());
		ESP8266_WIFIMode(BOTH_STATION_AND_ACCESPOINT);/* 3 = Both (AP and STA) */
		ESP8266_ConnectionMode(SINGLE);			/* 0 = Single; 1 = Multi */
		ESP8266_ApplicationMode(NORMAL);		/* 0 = Normal Mode; 1 = Transperant Mode */
		if(ESP8266_connected() == ESP8266_NOT_CONNECTED_TO_AP)
		ESP8266_JoinAccessPoint(SSID, PASSWORD);
		ESP8266_Start(0, DOMAIN, port);
while(1){
		byte = mfrc522_request(PICC_REQALL,str);
		LCDHexDumpXY(0,1,byte);
		LCDClear();

		if(byte == CARD_FOUND)
		{
			byte = mfrc522_get_card_serial(str);
			if(byte == CARD_FOUND)
			{
				for(byte=0;byte<8;byte++)
				{
				LCDHexDumpXY(byte*2,0,str[byte]);
				 Check=(Check*100)+str[byte];
				}
				DIOSetPinValue(1,0,1);
			if(Check==0xBBC8BF48E1199442){
				DIOSetPinValue(1,1,1);
				_delay_ms(1000);
				DIOSetPinValue(1,1,0);
			}
			else if(Check==0xBBC8BF51E6A9809E){
				DIOSetPinValue(1,2,1);
				_delay_ms(1000);
				DIOSetPinValue(1,2,0);
				}
			else
			{
				LCDWriteStringXY(0,0,"No Match!");
			}


				_delay_ms(2500);
				DIOSetPinValue(1,0,0);
			}
			else
			{
				LCDWriteStringXY(0,1,"Error");
			}
		}

		_delay_ms(1000);
	}
}





ISR (USART_RXC_vect)							/* Receive ISR routine */
{
	if(active==1){
	uint8_t oldsrg = SREG;
	RESPONSE_BUFFER[Counter] = UDR;				/* Copy data to buffer & increment counter */
	Counter++;
	if(Counter == DEFAULT_BUFFER_SIZE)
	Counter = 0;
	SREG = oldsrg;
	}
	if(active==2){
		uint8_t oldsrg = SREG;
			cli();
			RESPONSE_BUFFER1[Counter1] = UDR;
			Counter1++;
			if(Counter1 == DEFAULT_BUFFER_SIZE1){
				Counter1 = 0; pointer = 0;
			}
			SREG = oldsrg;
	}
	if(active==3){
		uint8_t oldsrg = SREG;
		cli();
		char received_char = UDR;

		if(received_char =='$'){ /* check for '$' */
		GGA_Index = 0;
		CommaCounter = 0;
		IsItGGAString = false;
		}
		else if(IsItGGAString == true){ /* if true save GGA info. into buffer */
		if(received_char == ',' ) GGA_Pointers[CommaCounter++] = GGA_Index; /* store instances of ',' in buffer */
		GGA_Buffer[GGA_Index++] = received_char;
		}
		else if(GGA_CODE[0] == 'G' && GGA_CODE[1] == 'G' && GGA_CODE[2] == 'A'){ /* check for GGA string */
		IsItGGAString = true;
		GGA_CODE[0] = 0; GGA_CODE[1] = 0; GGA_CODE[2] = 0;
		}
		else{
		GGA_CODE[0] = GGA_CODE[1]; GGA_CODE[1] = GGA_CODE[2]; GGA_CODE[2] = received_char;
		}
		SREG = oldsrg;
	}
}

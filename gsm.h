#ifndef GSM_H_
#define GSM_H_

#include <avr/io.h>									/* Include AVR std. library file */
#include <string.h>									/* Include string library */
#include <stdio.h>									/* Include standard library */
#include <stdlib.h>									/* Include standard library */
#include <stdbool.h>								/* Include standard boolean library */
#include <util/delay.h>								/* Include delay header file */
#include <avr/interrupt.h>							/* Include avr interrupt header file */
#include "USART.h"						/* Include USART header file */

#define DEFAULT_BUFFER_SIZE	200						/* Define default buffer size */
#define DEFAULT_TIMEOUT		10000					/* Define default timeout */
#define DEFAULT_CRLF_COUNT	2						/* Define default CRLF count */

#define POST				1						/* Define method */
#define GET					0

/* Select Demo */
#define GET_DEMO									/* Define GET demo */
//#define POST_DEMO									/* Define POST demo */

/* Define Required fields below */
#define AIO_SERVER			"io.adafruit.com"		/* Adafruit server */
#define AIO_BASE_URL		"/api/v2"				/* Base URL for api */
#define AIO_USERNAME		"Enter Username"		/* Enter username here */
#define AIO_KEY				"Enter AIO key"			/* Enter AIO key here */
#define AIO_FEED			"Enter Feed Key"		/* Enter feed key */

#define APN					"internet"				/* APN of network service provider */
#define USERNAME			""
#define PASS				""


#ifdef POST_DEMO
	#define SWITCH_PIN		PINA1
	#define SWITCH_ON		"ON"
	#define SWITCH_OFF		"OFF"
#endif

#ifdef GET_DEMO
	#define LED_PIN			PINB0
	#define LED_ON			PORTB |= (1 << LED_PIN);
	#define LED_OFF			PORTB &= ~(1 << LED_PIN);
#endif



enum SIM900_RESPONSE_STATUS							/* Enumerate response status */
{
	SIM900_RESPONSE_WAITING,
	SIM900_RESPONSE_FINISHED,
	SIM900_RESPONSE_TIMEOUT,
	SIM900_RESPONSE_BUFFER_FULL,
	SIM900_RESPONSE_STARTING,
	SIM900_RESPONSE_ERROR
};



void Read_Response();
void Start_Read_Response();
void Buffer_Flush();
void GetResponseBody(char*, uint16_t);
bool WaitForExpectedResponse(char*);
bool SendATandExpectResponse(char*, char*);
bool HTTP_Parameter(char*, char*);
bool SIM900HTTP_Start();
bool SIM900HTTP_Connect(char*, char*, char*);
bool HTTP_Init();
bool HTTP_Terminate();
bool HTTP_SetURL(char *);
bool HTTP_Connected();
bool HTTP_SetPost_json();
bool HTTP_Save();
bool HTTP_Data(char*);
bool HTTP_Action(char);
bool HTTP_Read(uint8_t, uint16_t);
uint8_t HTTP_Post(char* , uint16_t);
uint8_t HTTP_get(char *, uint16_t);
bool SIM900HTTP_Init();

#endif /* GSM_H_ */

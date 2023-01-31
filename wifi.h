#ifndef WIFI_H_
#define WIFI_H_
#define F_CPU 8000000UL			/* Define CPU Frequency e.g. here its Ext. 8MHz */
#include <avr/io.h>					/* Include AVR std. library file */
#include <util/delay.h>				/* Include Delay header file */
#include <stdbool.h>				/* Include standard boolean library */
#include <string.h>					/* Include string library */
#include <stdio.h>					/* Include standard IO library */
#include <stdlib.h>					/* Include standard library */
#include <avr/interrupt.h>			/* Include avr interrupt header file */
#include "USART.h"		/* Include USART header file */


#define DEFAULT_BUFFER_SIZE1		160
#define DEFAULT_TIMEOUT1			10000

/* Connection Mode */
#define SINGLE					0
#define MULTIPLE				1

/* Application Mode */
#define NORMAL					0
#define TRANSPERANT				1

/* Application Mode */
#define STATION							1
#define ACCESSPOINT						2
#define BOTH_STATION_AND_ACCESPOINT		3

/* Select Demo */
#define RECEIVE_DEMO				/* Define RECEIVE demo */
//#define SEND_DEMO					/* Define SEND demo */

/* Define Required fields shown below */
#define DOMAIN				"api.thingspeak.com"
#define port					"80"
#define API_WRITE_KEY		"C7JFHZY54GLCJY38"
#define CHANNEL_ID			"119922"

#define SSID				"ssid"
#define PASSWORD			"password"

enum ESP8266_Response_Status1{
	ESP8266_RESPONSE_WAITING,
	ESP8266_RESPONSE_FINISHED,
	ESP8266_RESPONSE_TIMEOUT,
	ESP8266_RESPONSE_BUFFER1_FULL,
	ESP8266_RESPONSE_STARTING,
	ESP8266_RESPONSE_ERROR
};

enum ESP8266_CONNECT_STATUS {
	ESP8266_CONNECTED_TO_AP,
	ESP8266_CREATED_TRANSMISSION,
	ESP8266_TRANSMISSION_DISCONNECTED,
	ESP8266_NOT_CONNECTED_TO_AP,
	ESP8266_CONNECT_UNKNOWN_ERROR
};

enum ESP8266_JOINAP_STATUS {
	ESP8266_WIFI_CONNECTED,
	ESP8266_CONNECTION_TIMEOUT,
	ESP8266_WRONG_PASSWORD,
	ESP8266_NOT_FOUND_TARGET_AP,
	ESP8266_CONNECTION_FAILED,
	ESP8266_JOIN_UNKNOWN_ERROR
};

void ESP_Read_Response(char*);
void ESP8266_Clear();
void ESP_Start_Read_Response(char*);
void ESP_GetResponseBody(char*, uint16_t);
bool ESP_WaitForExpectedResponse(char*);
bool ESP_SendATandExpectResponse(char*, char*);
bool ESP8266_ApplicationMode(uint8_t);
bool ESP8266_ConnectionMode(uint8_t);
bool ESP8266_Begin();
bool ESP8266_Close();
bool ESP8266_WIFIMode(uint8_t);
uint8_t ESP8266_JoinAccessPoint(char*, char*);
uint8_t ESP8266_connected();
uint8_t ESP8266_Start(uint8_t, char*, char*);
uint8_t ESP8266_Send(char*);
int16_t ESP8266_DataAvailable();
uint8_t ESP8266_DataRead();
uint16_t Read_Data(char*);

#endif /* WIFI_H_ */

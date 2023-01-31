#include"gsm.h"


 int8_t Response_Status, CRLF_COUNT = 0;
 uint16_t Counter = 0;
 uint32_t TimeOut = 0;
char RESPONSE_BUFFER[DEFAULT_BUFFER_SIZE];


void Read_Response()								/* Read response */
{
	static char CRLF_BUF[2];
	static char CRLF_FOUND;
	uint32_t TimeCount = 0, ResponseBufferLength;
	while(1)
	{
		if(TimeCount >= (DEFAULT_TIMEOUT+TimeOut))	/* Check timecount each time */
		{
			CRLF_COUNT = 0; TimeOut = 0;
			Response_Status = SIM900_RESPONSE_TIMEOUT;/* if its overflowed then make status timeout and return */
			return;
		}

		if(Response_Status == SIM900_RESPONSE_STARTING)
		{
			CRLF_FOUND = 0;
			memset(CRLF_BUF, 0, 2);
			Response_Status = SIM900_RESPONSE_WAITING;/* Make status waiting while reading */
		}
		ResponseBufferLength = strlen(RESPONSE_BUFFER);
		if (ResponseBufferLength)
		{
			_delay_ms(1);
			TimeCount++;
			if (ResponseBufferLength==strlen(RESPONSE_BUFFER))
			{
				for (uint16_t i=0;i<ResponseBufferLength;i++)
				{
					memmove(CRLF_BUF, CRLF_BUF + 1, 1);
					CRLF_BUF[1] = RESPONSE_BUFFER[i];
					if(!strncmp(CRLF_BUF, "\r\n", 2))
					{
						if(++CRLF_FOUND == (DEFAULT_CRLF_COUNT+CRLF_COUNT))
						{
							CRLF_COUNT = 0; TimeOut = 0;
							Response_Status = SIM900_RESPONSE_FINISHED;
							return;
						}
					}
				}
				CRLF_FOUND = 0;
			}
		}
		_delay_ms(1);
		TimeCount++;
	}
}

void Start_Read_Response()
{
	Response_Status = SIM900_RESPONSE_STARTING;
	do {
		Read_Response();
	} while(Response_Status == SIM900_RESPONSE_WAITING);/* Read response till response is waiting */
}

void Buffer_Flush()								/* Flush all variables */
{
	memset(RESPONSE_BUFFER, 0, DEFAULT_BUFFER_SIZE-1);
	Counter=0;
}

/* Remove CRLF and other default strings from response */
void GetResponseBody(char* Response, uint16_t ResponseLength)
{
	uint16_t i = 12;
	char buffer[5];
	while(Response[i] != '\r' && i < 100)
		++i;

	strncpy(buffer, Response + 12, (i - 12));
	ResponseLength = atoi(buffer);

	i += 2;
	uint16_t tmp = strlen(Response) - i;
	memcpy(Response, Response + i, tmp);

	if(!strncmp(Response + tmp - 6, "\r\nOK\r\n", 6))
	memset(Response + tmp - 6, 0, i + 6);
}

bool WaitForExpectedResponse(char* ExpectedResponse)
{
	Buffer_Flush();
	_delay_ms(200);
	Start_Read_Response();						/* First read response */
	if((Response_Status != SIM900_RESPONSE_TIMEOUT) && (strstr(RESPONSE_BUFFER, ExpectedResponse) != NULL))
		return true;							/* Return true for success */
	return false;								/* Else return false */
}

bool SendATandExpectResponse(char* ATCommand, char* ExpectedResponse)
{
	USART_SendString(ATCommand);				/* Send AT command to SIM900 */
	USART_TxChar('\r');
	return WaitForExpectedResponse(ExpectedResponse);
}

bool HTTP_Parameter(char* Parameter, char* Value)/* Set HTTP parameter and return response */
{

	USART_SendString("AT+HTTPPARA=\"");
	USART_SendString(Parameter);
	USART_SendString("\",\"");
	USART_SendString(Value);
	USART_SendString("\"\r");
	return WaitForExpectedResponse("OK");
}

bool SIM900HTTP_Start()							/* Check SIM900 board */
{
	for (uint8_t i=0;i<5;i++)
	{
		if(SendATandExpectResponse("ATE0","OK")||SendATandExpectResponse("AT","OK"))
		{
			HTTP_Parameter("CID","1");			/* set Bearer profile identifier */
			return true;
		}
	}
	return false;
}

bool SIM900HTTP_Connect(char* _APN, char* _USERNAME, char* _PASSWORD) /* Connect to GPRS */
{

	USART_SendString("AT+CREG?\r");
	if(!WaitForExpectedResponse("+CREG: 0,1"))
		return false;

	USART_SendString("AT+SAPBR=0,1\r");
	WaitForExpectedResponse("OK");

	USART_SendString("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r");
	WaitForExpectedResponse("OK");

	USART_SendString("AT+SAPBR=3,1,\"APN\",\"");
	USART_SendString(_APN);
	USART_SendString("\"\r");
	WaitForExpectedResponse("OK");

	USART_SendString("AT+SAPBR=3,1,\"USER\",\"");
	USART_SendString(_USERNAME);
	USART_SendString("\"\r");
	WaitForExpectedResponse("OK");

	USART_SendString("AT+SAPBR=3,1,\"PWD\",\"");
	USART_SendString(_PASSWORD);
	USART_SendString("\"\r");
	WaitForExpectedResponse("OK");

	USART_SendString("AT+SAPBR=1,1\r");
	return WaitForExpectedResponse("OK");
}

bool HTTP_Init()								/* Initiate HTTP */
{
	USART_SendString("AT+HTTPINIT\r");
	return WaitForExpectedResponse("OK");
}

bool HTTP_Terminate()							/* terminate HTTP */
{
	USART_SendString("AT+HTTPTERM\r");
	return WaitForExpectedResponse("OK");
}

bool HTTP_SetURL(char * url)					/* Set URL */
{
	return HTTP_Parameter("URL", url);
}

bool HTTP_Connected()							/* Check for connected */
{
	USART_SendString("AT+SAPBR=2,1\r");
	CRLF_COUNT = 2;										/* Make additional crlf count for response */
	return WaitForExpectedResponse("+SAPBR: 1,1");
}

bool HTTP_SetPost_json()						/* Set Json Application format for post */
{
	return HTTP_Parameter("CONTENT", "application/json");
}

bool HTTP_Save()								/* Save the application context */
{
	USART_SendString("AT+HTTPSCONT\r");
	return WaitForExpectedResponse("OK");
}

bool HTTP_Data(char* data)						/* Load HTTP data */
{
	char _buffer[25];
	sprintf(_buffer, "AT+HTTPDATA=%d,%d\r", strlen(data), 10000);
	USART_SendString(_buffer);

	if(WaitForExpectedResponse("DOWNLOAD"))
		return SendATandExpectResponse(data, "OK");
	else
		return false;
}

bool HTTP_Action(char method)					/* Select HTTP Action */
{
	if(method == GET)
		USART_SendString("AT+HTTPACTION=0\r");
	if(method == POST)
		USART_SendString("AT+HTTPACTION=1\r");
	return WaitForExpectedResponse("OK");
}

bool HTTP_Read(uint8_t StartByte, uint16_t ByteSize) /* Read HTTP response */
{
	char Command[25];
	sprintf(Command,"AT+HTTPREAD=%d,%d\r",StartByte,ByteSize);
	Command[25] = 0;
	USART_SendString(Command);

	CRLF_COUNT = 2;										/* Make additional crlf count for response */
	if(WaitForExpectedResponse("+HTTPREAD"))
	{
		GetResponseBody(RESPONSE_BUFFER, ByteSize);
		return true;
	}
	else
		return false;
}

uint8_t HTTP_Post(char* Parameters, uint16_t ResponseLength)
{
	HTTP_Parameter("CID","1");			/* set Bearer profile identifier */
	if(!(HTTP_Data(Parameters) && HTTP_Action(POST)))
	return SIM900_RESPONSE_TIMEOUT;

	bool status200 = WaitForExpectedResponse(",200,");

	if(Response_Status == SIM900_RESPONSE_TIMEOUT)
	return SIM900_RESPONSE_TIMEOUT;
	if(!status200)
	return SIM900_RESPONSE_ERROR;

	HTTP_Read(0, ResponseLength);
	return SIM900_RESPONSE_FINISHED;
}

uint8_t HTTP_get(char * _URL, uint16_t ResponseLength)
{
	HTTP_Parameter("CID","1");			/* set Bearer profile identifier */
	HTTP_Parameter("URL", _URL);
	HTTP_Action(GET);
	WaitForExpectedResponse("+HTTPACTION:0,");
	if(Response_Status == SIM900_RESPONSE_TIMEOUT)
	return SIM900_RESPONSE_TIMEOUT;

	HTTP_Read(0, ResponseLength);
	return SIM900_RESPONSE_FINISHED;
}

bool SIM900HTTP_Init()
{
	HTTP_Terminate();
	return HTTP_Init();
}





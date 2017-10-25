#include "SIMComAT.h"

void SIMComAT::begin(Stream& port)
{
	_port = &port;
	init();
}

void SIMComAT::flushInput()
{
	while (available()) read();
}

void SIMComAT::send() 
{
	flushInput();
	_port->println();
}

size_t SIMComAT::readLine(uint16_t timeout)
{
	uint8_t i = 0;

	while (timeout-- && i < BUFFER_SIZE)
	{
		while (available())
		{
			char c = read();
			if (c == '\r') continue;
			if (c == '\n')
			{
				if (i == 0) continue; //beginning of a new line
				else //end of the line
				{
					timeout = 0;
					break;
				}
			}
			replyBuffer[i] = c;
			i++;
		}

		delay(1);
	}

	replyBuffer[i] = 0; //string term

	RECEIVEARROW;
	PRINTLN(replyBuffer);

	return strlen(replyBuffer);
}

size_t SIMComAT::sendGetResponse(const char* msg, char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT)
{
	SENDARROW;
	print(msg);
	return sendGetResponse(response, timeout);
}

size_t SIMComAT::sendGetResponse(char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT) 
{
	send();
	readLine(timeout);
	
	size_t len = strlen(replyBuffer);
	if (response != NULL) {
		size_t maxLen = min(len + 1, BUFFER_SIZE - 1);

		strncpy(response, replyBuffer, maxLen);
		response[maxLen] = '\0';
	}

	return len;
}

bool SIMComAT::sendAssertResponse(const char* msg, const char* expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT)
{
	if (!sendGetResponse(msg, NULL, timeout)) return false;
	return assertResponse(expectedResponse);
}

bool SIMComAT::sendAssertResponse(const char* expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT)
{
	if (!sendGetResponse(NULL, timeout)) return false;
	return assertResponse(expectedResponse);
}

bool SIMComAT::assertResponse(const char* expectedResponse)
{
	PRINT(F("assertResponse : ["));
	PRINT(replyBuffer);
	PRINT("], [");
	PRINT(expectedResponse);
	PRINTLN("]");

	return !strcasecmp(replyBuffer, expectedResponse);
}

char* SIMComAT::find(const char* str, char divider, uint8_t index);
{
	PRINT("parse : [");
	PRINT(str);
	PRINT(", ");
	PRINT(divider);
	PRINT(", ");
	PRINT(index);
	PRINTLN("]");

	const char* p = strchr(str, ':');
	if (p == NULL) return NULL;

	for (uint8_t i = 0; i < index; i++)
	{
		p = strchr(p, divider);
		if (p == NULL) return NULL;
		p++;
	}

	PRINT("parse : [");
	PRINT(divider);
	PRINT(", ");
	PRINT(index);
	PRINT(", ");
	PRINT(replyBuffer);
	PRINT("], [");
	PRINT(*result);
	PRINTLN("]");

	return p;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, uint8_t* result)
{
	uint16_t tmpResult;
	if (!parseReply(str, divider, index, &tmpResult)) return false;

	*result = (uint8_t)tmpResult;
	return true;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, uint16_t* result)
{
	char* p = find(str, divider, index);
	if (p == NULL) return false;

	*result = (uint16_t)atoi(p);
	return true;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, float* result)
{
	char* p = find(str, divider, index);
	if (p == NULL) return false;

	*result = atof(p);
	return true;
}

bool SIMComAT::parseReply(char divider, uint8_t index, uint8_t* result) 
{
	return parse(replyBuffer, divider, index, result);
}

bool SIMComAT::parseReply(char divider, uint8_t index, uint16_t* result) 
{
	return parse(replyBuffer, divider, index, result);
}

bool SIMComAT::parseReply(char divider, uint8_t index, float* result)
{
	return parse(replyBuffer, divider, index, result);
}

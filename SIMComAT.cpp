#include "SIMComAT.h"
#include <errno.h>

void SIMComAT::begin(Stream& port)
{
	_port = &port;
	_output.begin(LOG_LEVEL_VERBOSE, this, false);
#ifdef _SIM808_DEBUG
	_debug.begin(LOG_LEVEL_VERBOSE, &Serial, false);
#endif // _SIM808_DEBUG

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

size_t SIMComAT::readLine(uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT)
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
	SIM808_PRINT(replyBuffer);
	SIM808_PRINT(CR);

	return strlen(replyBuffer);
}

size_t SIMComAT::sendGetResponse(const __FlashStringHelper* msg, char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT)
{
	SENDARROW;
	print(msg);
	return sendGetResponse(response, timeout);
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

bool SIMComAT::sendAssertResponse(const __FlashStringHelper *msg, const __FlashStringHelper *expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT)
{
	if (!sendGetResponse(msg, NULL, timeout)) return false;
	return assertResponse(expectedResponse);
}

bool SIMComAT::sendAssertResponse(const __FlashStringHelper *expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT)
{
	if (!sendGetResponse(NULL, timeout)) return false;
	return assertResponse(expectedResponse);
}

bool SIMComAT::assertResponse(const __FlashStringHelper *expectedResponse)
{
	SIM808_PRINT_P("assertResponse : [%s], [%S]", replyBuffer, expectedResponse);
	return !strcasecmp_P(replyBuffer, (char *)expectedResponse);
}

char* SIMComAT::find(const char* str, char divider, uint8_t index)
{
	SIM808_PRINT_P("find : [%s, %c, %i]", str, divider, index);

	char* p = strchr(str, ':');
	if (p == NULL) return NULL;

	p++;
	for (uint8_t i = 0; i < index; i++)
	{
		p = strchr(p, divider);
		if (p == NULL) return NULL;
		p++;
	}

	SIM808_PRINT_P("find : [%s, %c, %i], [%s]", str, divider, index, p);

	return p;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, uint8_t* result)
{
	uint16_t tmpResult;
	if (!parse(str, divider, index, &tmpResult)) return false;

	*result = (uint8_t)tmpResult;
	return true;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, int8_t* result)
{
	int16_t tmpResult;
	if (!parse(str, divider, index, &tmpResult)) return false;

	*result = (int8_t)tmpResult;
	return true;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, uint16_t* result)
{
	char* p = find(str, divider, index);
	if (p == NULL) return false;

	errno = 0;
	//*result = (uint16_t)atol(p);
	*result = strtoul(p, NULL, 10);

	return errno == 0;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, int16_t* result)
{	
	char* p = find(str, divider, index);
	if (p == NULL) return false;

	errno = 0;
	//*result = atol(p);
	*result = strtol(p, NULL, 10);
	
	return errno == 0;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, float* result)
{
	char* p = find(str, divider, index);
	if (p == NULL) return false;

	errno = 0;
	*result = strtod(str, NULL);

	return errno == 0;
}

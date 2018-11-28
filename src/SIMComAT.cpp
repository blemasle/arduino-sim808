#include "SIMComAT.h"
#include <errno.h>

void SIMComAT::begin(Stream& port)
{
	_port = &port;
	_output.begin(LOG_LEVEL_VERBOSE, this, false);
#if _SIM808_DEBUG
	_debug.begin(LOG_LEVEL_VERBOSE, &Serial, false);
#endif // _SIM808_DEBUG
}

void SIMComAT::flushInput(); {
	uint16_t timeout = 0;
	while(readNext(&timeout));
}

size_t SIMComAT::readNext(uint16_t * timeout)
{
	uint8_t i = 0;
	memset(replyBuffer, 0, BUFFER_SIZE);

	uint16_t start = millis();

	do {
		while(i < BUFFER_SIZE - 1 && available()) {
			char c = read();
			replyBuffer[i] = c;
			i++;

			if(c == '\n') {
				timeout = 0; //forcing the outer loop to break
				break;
			}
		}
	} while(i < BUFFER_SIZE - 1 && millis() - start < timeout);

	replyBuffer[i] = '\0';

	RECEIVEARROW;
	SIM808_PRINT(replyBuffer);
	if(i > 0 && replyBuffer[i - 1] == '\n')
		SIM808_PRINT(CR);

	return strlen(replyBuffer);
}

int8_t SIMComAT::waitResponse(uint16_t timeout, 
	Sim808ConstStr s1 = SFP(TOKEN_OK),
	Sim808ConstStr s2 = SFP(TOKEN_ERROR),
	Sim808ConstStr s3 = NULL,
	Sim808ConstStr s4 = NULL)
{
	uint16_t start = millis();
	Sim808ConstStr wantedTokens[4] = { s1, s2, s3, s4 };

	do {
		readNext(timeout - (millis() - start));
		for(uint8_t i = 0; i < 4; i++) {
			if(wantedTokens[i] && strstr_P(replyBuffer, SFPT(wantedTokens[i])) == replyBuffer) return i;
		}
	} while(!millis() - start < timeout);

	return -1;
}

size_t SIMComAT::copyCurrentLine(char * dst, uint16_t shift = 0)
{
	
	size_t lenght = 0;
	char *p = response;
	
	p += safeCopy(replyBuffer + shift, p);

	while(strstr_P(replyBuffer, TOKEN_NL) == 0) {
		readNext();
		p += safeCopy(replyBuffer, p);
	}

	return response - p;
}

size_t SIMComAT::safeCopy(const char *src, char *dst)
{
	size_t len = strlen(src);
	if (dst != NULL) {
		size_t maxLen = min(len + 1, BUFFER_SIZE - 1);
		strlcpy(dst, src, maxLen);
	}

	return len;
}

char* SIMComAT::find(const char* str, char divider, uint8_t index)
{
	SIM808_PRINT_P("find : [%s, %c, %i]", str, divider, index);

	char* p = strchr(str, ':');
	if (p == NULL) p = strchr(str, str[0]); //ditching eventual response header

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
	*result = strtoul(p, NULL, 10);

	return errno == 0;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, int16_t* result)
{	
	char* p = find(str, divider, index);
	if (p == NULL) return false;

	errno = 0;
	*result = strtol(p, NULL, 10);
	
	return errno == 0;
}

bool SIMComAT::parse(const char* str, char divider, uint8_t index, float* result)
{
	char* p = find(str, divider, index);
	if (p == NULL) return false;

	errno = 0;
	*result = strtod(p, NULL);

	return errno == 0;
}

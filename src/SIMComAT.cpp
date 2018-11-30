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

void SIMComAT::flushInput() {
	uint16_t timeout = 0;
	while(readNext(&timeout));
}

size_t SIMComAT::readNext(uint16_t * timeout)
{
	uint8_t i = 0;
	bool gotNewLine = false;
	memset(replyBuffer, 0, BUFFER_SIZE);

	do {
		while(i < BUFFER_SIZE - 1 && available()) {
			char c = read();
			replyBuffer[i] = c;
			i++;

			if(c == '\n') {
				gotNewLine = true; //forcing the outer loop to break
				break;
			}
		}

		delay(1);
		if(*timeout) (*timeout)--;
	} while(!gotNewLine && i < BUFFER_SIZE - 1 && *timeout);

	replyBuffer[i] = '\0';

	if(i) {
		RECEIVEARROW;
		SIM808_PRINT(replyBuffer);
	}

	return i;
}

int8_t SIMComAT::waitResponse(uint16_t timeout, 
	ATConstStr s1 = TO_F(TOKEN_OK),
	ATConstStr s2 = TO_F(TOKEN_ERROR),
	ATConstStr s3 = NULL,
	ATConstStr s4 = NULL)
{
	ATConstStr wantedTokens[4] = { s1, s2, s3, s4 };
	size_t length;

	do {
		length = readNext(&timeout);

		if(!length) continue; 					//read nothing
		if(wantedTokens[0] == NULL) return 0;	//looking for a line with any content

		for(uint8_t i = 0; i < 4; i++) {
			if(wantedTokens[i]) {
				char *p = strstr_P(replyBuffer, TO_P(wantedTokens[i]));
				if(replyBuffer == p) return i;				
			}
		}
	} while(timeout);

	return -1;
}

size_t SIMComAT::copyCurrentLine(char * dst, uint16_t shift = 0)
{
	char *p = dst;
	p += safeCopy(replyBuffer + shift, p);

	while(strstr_P(replyBuffer, TOKEN_NL) == 0) {
		waitResponse((uint16_t)0, NULL);
		p += safeCopy(replyBuffer, p);
	}

	if(*(p - 1) == '\n') *(p - 1) = '\0';
	return strlen(dst);
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
	char* p = strchr(str, ':');
	if (p == NULL) p = strchr(str, str[0]); //ditching eventual response header

	p++;
	for (uint8_t i = 0; i < index; i++)
	{
		p = strchr(p, divider);
		if (p == NULL) return NULL;
		p++;
	}

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

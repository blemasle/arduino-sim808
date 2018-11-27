#include "SIM808.h"

TOKEN_TEXT(HTTP_DATA, "+HTTPDATA");
TOKEN_TEXT(HTTP_ACTION, "+HTTPACTION");
TOKEN_TEXT(HTTP_READ, "+HTTPREAD");
TOKEN_TEXT(HTTP_INIT, "+HTTPINIT");
TOKEN_TEXT(HTTP_SSL, "+HTTPSSL");
TOKEN_TEXT(HTTP_TERM, "+HTTPTERM");
TOKEN(DOWNLOAD);

AT_COMMAND_PARAMETER(HTTP, CONTENT);
AT_COMMAND_PARAMETER(HTTP, REDIR);
AT_COMMAND_PARAMETER(HTTP, CID);
AT_COMMAND_PARAMETER(HTTP, URL);
AT_COMMAND_PARAMETER(HTTP, UA);


uint16_t SIM808::httpGet(const char *url, char *response, size_t responseSize)
{
	uint16_t statusCode = 0;
	size_t dataSize = 0;

	bool result = setupHttpRequest(url) &&
		fireHttpRequest(SIM808_HTTP_ACTION::GET, &statusCode, &dataSize) &&
		readHttpResponse(response, min(responseSize, dataSize)) &&
		httpEnd();

	return statusCode;
}

uint16_t SIM808::httpPost(const char *url, const __FlashStringHelper *contentType, const char *body, char *response, size_t responseSize)
{
	uint16_t statusCode = 0;
	size_t dataSize = 0;

	bool result = setupHttpRequest(url) &&
		setHttpParameter(SFP(AT_COMMAND_PARAMETER_HTTP_CONTENT), contentType) &&
		setHttpBody(body) &&
		fireHttpRequest(SIM808_HTTP_ACTION::POST, &statusCode, &dataSize) &&
		readHttpResponse(response, min(dataSize, responseSize)) &&
		httpEnd();

	return statusCode;
}

__attribute__((__optimize__("O2"))) //workaround for compiler bug about 'NO_REGS'
bool SIM808::setupHttpRequest(const char* url)
{
	httpEnd();

	return httpInit() &&
		setHttpParameter(SFP(AT_COMMAND_PARAMETER_HTTP_REDIR), 1) &&
		setHttpParameter(SFP(AT_COMMAND_PARAMETER_HTTP_CID), 1) &&
		setHttpParameter(SFP(AT_COMMAND_PARAMETER_HTTP_URL), url) &&
		(url[4] != 's' || (sendAT(SFP(TOKEN_HTTP_SSL), SFP(TOKEN_WRITE), 1), waitResponse() == 0)) &&
		(_userAgent == NULL || setHttpParameter(SFP(AT_COMMAND_PARAMETER_HTTP_UA), _userAgent));
}

bool SIM808::httpInit()
{
	return (sendAT(SFP(TOKEN_HTTP_INIT)), waitResponse() == 0);
}

bool SIM808::httpEnd()
{
	return (sendAT(SFP(TOKEN_HTTP_TERM)), waitResponse() == 0);
}

bool SIM808::setHttpBody(const char* body)
{
	sendAT(SFP(TOKEN_HTTP_DATA), SFP(TOKEN_WRITE), strlen(body), 10000L);
	
	if(!waitResponse(SFP(TOKEN_DOWNLOAD)) == 0 ||
		waitResponse() != 0) return false;

	SENDARROW;
	print(body);
}

bool SIM808::fireHttpRequest(const SIM808_HTTP_ACTION action, uint16_t *statusCode, size_t *dataSize)
{
	sendAT(SFP(TOKEN_HTTP_ACTION), SFP(TOKEN_WRITE), (uint8_t)action);

	return waitResponse(HTTP_TIMEOUT, SFP(TOKEN_HTTP_ACTION)) == 0 &&
		parseReply(',', (uint8_t)SIM808_HTTP_ACTION_RESPONSE::STATUS_CODE, statusCode) &&
		parseReply(',', (uint8_t)SIM808_HTTP_ACTION_RESPONSE::DATA_LEN, dataSize);
}

bool SIM808::readHttpResponse(char *response, size_t responseSize)
{
	int16_t length;
	int16_t i = 0;

	sendAT(SFP(TOKEN_HTTP_READ), SFP(TOKEN_WRITE), 0, responseSize);

	if(waitResponse(SFP(TOKEN_HTTP_READ)) != 0 ||
		!parseReply(',', 0, &length))
		return false;

	while (length > 0) {
		while (available()) {
			char c = read();
			*response = c;
			response++;
			length--;
			if (!length) break;
		}
	}

	*response = '\0';

	return waitResponse() == 0;
}

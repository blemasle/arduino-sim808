#include "SIM808.h"

SIM808_COMMAND(HTTP_SET_PARAMETER_STRING, "AT+HTTPPARA=\"%S\",\"%s\"");
SIM808_COMMAND(HTTP_SET_PARAMETER_STRING_PROGMEM, "AT+HTTPPARA=\"%S\",\"%S\"");
SIM808_COMMAND(HTTP_SET_PARAMETER_INT, "AT+HTTPPARA=\"%S\",\"%d\"");
SIM808_COMMAND(HTTP_SET_HTTP_DATA, "AT+HTTPDATA=%d,%d");
SIM808_COMMAND(HTTP_ACTION, "AT+HTTPACTION=%d");
SIM808_COMMAND(HTTP_READ, "AT+HTTPREAD=%d,%d");
SIM808_COMMAND(HTTP_INIT, "AT+HTTPINIT");
SIM808_COMMAND(HTTP_END, "AT+HTTPTERM");

SIM808_COMMAND_PARAMETER(HTTP, CONTENT);
SIM808_COMMAND_PARAMETER(HTTP, REDIR);
SIM808_COMMAND_PARAMETER(HTTP, CID);
SIM808_COMMAND_PARAMETER(HTTP, URL);
SIM808_COMMAND_PARAMETER(HTTP, UA);

SIM808_TOKEN(DOWNLOAD);

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
		setHttpParameter(PSTRPTR(SIM808_COMMAND_PARAMETER_HTTP_CONTENT), contentType) &&
		setHttpBody(body) &&
		fireHttpRequest(SIM808_HTTP_ACTION::POST, &statusCode, &dataSize) &&
		readHttpResponse(response, min(dataSize, responseSize)) &&
		httpEnd();

	return statusCode;
}

__attribute__((__optimize__("O2"))) //workaround for compiler bug about 'NO_REGS
bool SIM808::setupHttpRequest(const char* url)
{
	httpEnd();

	return httpInit() &&
		setHttpParameter(PSTRPTR(SIM808_COMMAND_PARAMETER_HTTP_REDIR), 1) &&
		setHttpParameter(PSTRPTR(SIM808_COMMAND_PARAMETER_HTTP_CID), 1) &&
		setHttpParameter(PSTRPTR(SIM808_COMMAND_PARAMETER_HTTP_URL), url) &&
		(_userAgent == NULL || setHttpParameter(PSTRPTR(SIM808_COMMAND_PARAMETER_HTTP_UA), _userAgent));
}

bool SIM808::httpInit()
{
	return sendAssertResponse(PSTRPTR(SIM808_COMMAND_HTTP_INIT), _ok);
}

bool SIM808::httpEnd()
{
	return sendAssertResponse(PSTRPTR(SIM808_COMMAND_HTTP_END), _ok);
}

bool SIM808::setHttpParameter(const __FlashStringHelper* parameter, const __FlashStringHelper* value)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_HTTP_SET_PARAMETER_STRING_PROGMEM), parameter, value);

	return sendAssertResponse(_ok);
}

bool SIM808::setHttpParameter(const __FlashStringHelper* parameter, const char* value)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_HTTP_SET_PARAMETER_STRING), parameter, value);

	return sendAssertResponse(_ok);
}

bool SIM808::setHttpParameter(const __FlashStringHelper* parameter, const int8_t value)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_HTTP_SET_PARAMETER_INT), parameter, value);

	return sendAssertResponse(_ok);
}

bool SIM808::setHttpBody(const char* body)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_HTTP_SET_HTTP_DATA), strlen(body), (uint16_t)10000);

	if (!sendAssertResponse(PSTRPTR(SIM808_TOKEN_DOWNLOAD))) return false;

	SENDARROW;
	print(body);

	readLine();
	return assertResponse(_ok);
}

bool SIM808::fireHttpRequest(const SIM808_HTTP_ACTION action, uint16_t *statusCode, size_t *dataSize)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_HTTP_ACTION), action);

	if (!sendAssertResponse(_ok)) return false;

	readLine(_httpTimeout);

	return parseReply(',', (uint8_t)SIM808_HTTP_ACTION_RESPONSE::STATUS_CODE, statusCode) &&
		parseReply(',', (uint8_t)SIM808_HTTP_ACTION_RESPONSE::DATA_LEN, dataSize);
}

bool SIM808::readHttpResponse(char *response, size_t responseSize)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_HTTP_READ), (uint8_t)0, responseSize);

	send();
	readLine();

	int16_t length = responseSize;
	int16_t i = 0;
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

	readLine();
	return assertResponse(_ok);
}

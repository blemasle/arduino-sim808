#include "SIM808.h"

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

uint16_t SIM808::httpPost(const char *url, const char *contentType, const char *body, char *response, size_t responseSize)
{
	uint16_t statusCode = 0;
	size_t dataSize = 0;

	bool result = setupHttpRequest(url) &&
		setHttpParameter("CONTENT", contentType) &&
		setHttpBody(body) &&
		fireHttpRequest(SIM808_HTTP_ACTION::POST, &statusCode, &dataSize) &&
		readHttpResponse(response, min(dataSize, responseSize)) &&
		httpEnd();

	return statusCode;
}

bool SIM808::setupHttpRequest(const char* url)
{
	httpEnd();

	return httpInit() &&
		setHttpParameter("REDIR", 1) &&
		setHttpParameter("CID", 1) &&
		setHttpParameter("URL", url) &&
		(_userAgent == NULL || setHttpParameter("UA", _userAgent));
}

bool SIM808::httpInit()
{
	return sendAssertResponse("AT+HTTPINIT", _ok);
}

bool SIM808::httpEnd()
{
	return sendAssertResponse("AT+HTTPTERM", _ok);
}

bool SIM808::setHttpParameter(const char* parameter, const char* value)
{
	SENDARROW;
	print("AT+HTTPPARA=\"");
	print(parameter);
	print("\",\"");
	print(value);
	print('"');

	return sendAssertResponse(_ok);
}

bool SIM808::setHttpParameter(const char* parameter, const int8_t value)
{
	SENDARROW;
	print("AT+HTTPPARA=\"");
	print(parameter);
	print("\",");
	print(value);

	return sendAssertResponse(_ok);
}

bool SIM808::setHttpBody(const char* body)
{
	SENDARROW;
	print("AT+HTTPDATA=");
	print(strlen(body));
	print(',');
	print((uint16_t)10000);

	if (!sendAssertResponse("DOWNLOAD")) return false;

	SENDARROW;
	print(body);

	readLine(1000);
	return assertResponse(_ok);
}

bool SIM808::fireHttpRequest(const SIM808_HTTP_ACTION action, uint16_t *statusCode, size_t *dataSize)
{
	SENDARROW;
	print("AT+HTTPACTION=");
	print((uint8_t)action);

	if (!sendAssertResponse(_ok)) return false;

	readLine(_httpTimeout);

	return parseReply(',', (uint8_t)SIM808_HTTP_ACTION_RESPONSE::STATUS_CODE, statusCode) &&
		parseReply(',', (uint8_t)SIM808_HTTP_ACTION_RESPONSE::DATA_LEN, dataSize);
}

bool SIM808::readHttpResponse(char *response, size_t responseSize)
{
	SENDARROW;
	print("AT+HTTPREAD=");
	print((uint8_t)0);
	print(',');
	print(responseSize);

	send();
	readLine(1000);

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

	readLine(1000);
	return assertResponse(_ok);
}

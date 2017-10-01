#include "SIM808.h"

SIM808::SIM808(int8_t resetPin)
{
	_resetPin = resetPin;
	_ok = "OK";
}

SIM808::~SIM808() { }

#pragma region Public functions

void SIM808::init()
{
	reset();
	waitForReady();
	delay(3000);

	setEcho(SIM808_BOOL::OFF);
	setEcho(SIM808_BOOL::OFF);
}

void SIM808::reset()
{
	pinMode(_resetPin, OUTPUT);
	digitalWrite(_resetPin, HIGH);
	delay(10);
	digitalWrite(_resetPin, LOW);
	delay(200);

	digitalWrite(_resetPin, HIGH);
}

void SIM808::waitForReady()
{
	do
	{
		PRINTLN(F("Waiting for echo..."));
	} while (!sendAssertResponse("AT", "AT"));

	do
	{
		PRINTLN(F("Waiting for RDY..."));
		readLine(1000);
	} while (!assertResponse("RDY"));

}

bool SIM808::setEcho(SIM808_BOOL mode)
{
	SENDARROW;
	print("ATE");
	print(mode);

	return sendAssertResponse(_ok);
}

bool SIM808::simUnlock(const char* pin)
{
	SENDARROW;
	print("AT+CPIN=");
	print(pin);

	return sendAssertResponse(_ok, 5000);
}

size_t SIM808::getSimState(char *state)
{
	SENDARROW;
	print("AT+CPIN?");
	sendGetResponse(state);

	readLine(1000);
	return strlen(state);
}

size_t SIM808::getImei(char *imei)
{
	SENDARROW;
	print("AT+GSN");
	sendGetResponse(imei);

	readLine(1000);
	return assertResponse(_ok) ?
		strlen(imei) :
		0;
}

bool SIM808::setSmsMessageFormat(SIM808_SMS_MESSAGE_FORMAT format)
{
	SENDARROW;
	print("AT+CMGF=");
	print(format);

	return sendAssertResponse(_ok);
}

bool SIM808::sendSms(const char *addr, const char *msg)
{
	if (!setSmsMessageFormat(SIM808_SMS_MESSAGE_FORMAT::TEXT)) return false;

	SENDARROW;
	print("AT+CMGS=\"");
	print(addr);
	print('"');

	if (!sendAssertResponse("> ")) return false;

	SENDARROW;
	println(msg);
	println();
	print((char)0x1A);

	readLine(10000);
	if (strstr(_replyBuffer, "+CMGS") == 0) return false;

	readLine(1000);
	if (!assertResponse(_ok)) return false;

	return true;
}

size_t SIM808::sendCommand(const char *cmd, char *response)
{
	flushInput();
	print(cmd);
	sendGetResponse(response);
	readLine(1000);

	return strlen(response);
}

#pragma endregion

#pragma region GPRS

bool SIM808::setBearerSetting(const char* parameter, const char* value)
{
	SENDARROW;
	print("AT+SAPBR=3,1,\"");
	print(parameter);
	print("\",\"");
	print(value);
	print('"');

	return sendAssertResponse(_ok);
}

bool SIM808::enableGprs(const char *apn)
{
	return enableGprs(apn, NULL, NULL);
}

bool SIM808::enableGprs(const char *apn, const char* user, const char *password)
{
	bool success = sendAssertResponse("AT+CIPSHUT", "SHUT OK", 65000) &&
		sendAssertResponse("AT+CGATT=1", _ok, 10000) &&
		setBearerSetting("CONTYPE", "GPRS") &&
		setBearerSetting("APN", apn) &&
		(user == NULL || setBearerSetting("USER", user)) &&
		(password == NULL || setBearerSetting("PWD", password));

	if (!success) return false;

	SENDARROW;
	print("AT+CSTT=\"");
	print(apn);
	print('"');

	if (user) {
		print(",\"");
		print(user);
		print('"');
	}

	if (password) {
		print(",\"");
		print(password);
		print('"');
	}

	if (!sendAssertResponse(_ok)) return false;

	return sendAssertResponse("AT+SAPBR=1,1", _ok, 65000) &&
		sendAssertResponse("AT+CIICR", _ok, 65000);

}

bool SIM808::disableGprs()
{
	sendAssertResponse("AT+CIPSHUT", "SHUT OK", 65000);
	sendAssertResponse("AT+SAPBR=0,1", _ok, 65000);
	sendAssertResponse("AT+CGATT=0", _ok, 10000);

	return true;
}

SIM808RegistrationStatus SIM808::getNetworkRegistrationStatus()
{
	uint8_t n;
	uint8_t stat;
	SIM808RegistrationStatus result = { n = -1, stat = -1 };

	SENDARROW;
	print("AT+CGREG?");

	send();
	readLine(1000);
	if (strstr(_replyBuffer, "+CGREG") == 0) return result;

	if (!parseReply(',', SIM9082_REGISTRATION_STATUS_RESPONSE::N, &n) ||
		!parseReply(',', SIM9082_REGISTRATION_STATUS_RESPONSE::STAT, &stat)) return result;

	readLine(1000);
	if (!assertResponse(_ok)) return result;

	PRINT("getNetworkRegistrationStatus : ");
	PRINT(n);
	PRINT(", ");
	PRINTLN(stat);

	result.n = n;
	result.stat = stat;

	return result;
}

#pragma endregion

#pragma region HTTP

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

	if (sendAssertResponse("DOWNLOAD")) return false;

	SENDARROW;
	print(body);

	readLine(1000);
	return assertResponse(_ok);
}

bool SIM808::fireHttpRequest(const SIM808_HTTP_ACTION action, uint16_t *statusCode, size_t *dataSize)
{
	SENDARROW;
	print("AT+HTTPACTION=");
	print(action);

	if (!sendAssertResponse(_ok)) return false;

	readLine(_httpTimeout);

	return parseReply(',', SIM808_HTTP_ACTION_RESPONSE::STATUS_CODE, statusCode) &&
		parseReply(',', SIM808_HTTP_ACTION_RESPONSE::DATA_LEN, dataSize);
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

#pragma endregion

#pragma region GPS

bool SIM808::enableGps()
{
	bool currentState = false;
	if (!getGpsPowerState(&currentState) || currentState) return false;

	SENDARROW;
	print("AT+CGNSPWR=1");

	return sendAssertResponse(_ok);
}

bool SIM808::disableGps()
{
	bool currentState = false;
	if (!getGpsPowerState(&currentState) || !currentState) return false;

	SENDARROW;
	print("AT+CGNSPWR=0");

	return sendAssertResponse(_ok);
}

bool SIM808::getGpsPosition(char *response)
{
	SENDARROW;
	print("AT+CGNSINF");

	sendGetResponse(response);
	return true;
}

bool SIM808::getGpsPowerState(bool *state)
{
	uint8_t result;
	SENDARROW;
	print("AT+CGNSPWR?");

	send();
	readLine(1000);
	if (strstr(_replyBuffer, "+CGNSPWR") == 0) return false;

	if (!parseReply(',', 0, &result)) return false;

	*state = result;

	readLine(1000);
	return assertResponse(_ok);
}

#pragma endregion

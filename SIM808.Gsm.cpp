#include "SIM808.h"

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
#include "SIM808.h"

SIM808_COMMAND(SET_CPIN, "AT+CPIN=%s");
SIM808_COMMAND(GET_CPIN, "AT+CPIN?");

SIM808_COMMAND(GET_IMEI, "AT+GSN");

SIM808_COMMAND(SET_SMS_MESSAGE_FORMAT, "AT+CMGF=%d");
SIM808_COMMAND(SEND_SMS, "AT+CMGS=\"%s\"");

bool SIM808::simUnlock(const char* pin)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_SET_CPIN), pin);

	return sendAssertResponse(_ok, 5000);
}

size_t SIM808::getSimState(char *state)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_GET_CPIN));
	sendGetResponse(state);

	readLine(1000);
	return strlen(state);
}

size_t SIM808::getImei(char *imei)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_GET_IMEI));
	sendGetResponse(imei);

	readLine(1000);
	return assertResponse(_ok) ?
		strlen(imei) :
		0;
}

bool SIM808::setSmsMessageFormat(SIM808_SMS_MESSAGE_FORMAT format)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_SET_SMS_MESSAGE_FORMAT), format);

	return sendAssertResponse(_ok);
}

bool SIM808::sendSms(const char *addr, const char *msg)
{
	if (!setSmsMessageFormat(SIM808_SMS_MESSAGE_FORMAT::TEXT)) return false;

	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_SEND_SMS), addr);

	if (!sendAssertResponse("> ")) return false;

	SENDARROW;
	println(msg);
	println();
	print((char)0x1A);

	readLine(10000);
	if (strstr_P(replyBuffer, PSTR("+CMGS")) == 0) return false;

	readLine(1000);
	if (!assertResponse(_ok)) return false;

	return true;
}
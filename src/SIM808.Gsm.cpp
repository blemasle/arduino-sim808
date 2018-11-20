#include "SIM808.h"

SIM808_COMMAND(SET_CPIN, "AT+CPIN=%s");
SIM808_COMMAND(GET_CPIN, "AT+CPIN?");

SIM808_COMMAND(GET_IMEI, "AT+GSN");

SIM808_COMMAND(GET_SIGNAL_QUALITY, "AT+CSQ");

SIM808_COMMAND(SET_SMS_MESSAGE_FORMAT, "AT+CMGF=%d");
SIM808_COMMAND(SEND_SMS, "AT+CMGS=\"%s\"");

const char SIM808_COMMAND_SEND_SMS_RESPONSE[] PROGMEM = "+CMGS:";
const char SIM808_COMMAND_GET_SIGNAL_QUALITY_RESPONSE[] PROGMEM = "+CSQ:";

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

	readLine();
	return strlen(state);
}

size_t SIM808::getImei(char *imei)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_GET_IMEI));
	sendGetResponse(imei);

	readLine();
	return assertResponse(_ok) ?
		strlen(imei) :
		0;
}

SIM808SignalQualityReport SIM808::getSignalQuality()
{
	SIM808SignalQualityReport report = {99, 99, 1};
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_GET_SIGNAL_QUALITY));

	send();
	readLine();

	if (strstr_P(replyBuffer, SIM808_COMMAND_GET_SIGNAL_QUALITY_RESPONSE) == 0) return report;

	uint8_t quality;
	uint8_t errorRate;
	if (!parseReply(',', (uint8_t)SIM808_SIGNAL_QUALITY_RESPONSE::SIGNAL_STRENGTH, &quality) ||
		!parseReply(',', (uint8_t)SIM808_SIGNAL_QUALITY_RESPONSE::BIT_ERROR_RATE, &errorRate)) return report;

	report.rssi = quality;
	report.ber = errorRate;

	if (quality == 0) report.attenuation = -115;
	else if (quality == 1) report.attenuation = -111;
	else if (quality == 31) report.attenuation = -52;
	else if (quality > 31) report.attenuation = 1;
	else report.attenuation = map(quality, 2, 30, -110, -54);

	return report;
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

	if (!sendAssertResponse(F("> "))) return false;

	SENDARROW;
	println(msg);
	flushInput(); //flushing all "> " that might have come because of a multiline message
	print((char)0x1A);

	readLine(10000);
	if (strstr_P(replyBuffer, SIM808_COMMAND_SEND_SMS_RESPONSE) == 0) return false;

	readLine();
	if (!assertResponse(_ok)) return false;

	return true;
}
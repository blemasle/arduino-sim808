#include "SIM808.h"

TOKEN_TEXT(CPIN, "+CPIN");
TOKEN_TEXT(CSQ, "+CSQ");
TOKEN_TEXT(CMGS, "+CMGS");

bool SIM808::simUnlock(const char* pin)
{
	SENDARROW;
	sendAT(SFP(TOKEN_CPIN), SFP(TOKEN_WRITE), pin);

	return waitResponse(5000L) == 0;
}

size_t SIM808::getSimState(char *state)
{
	SENDARROW;
	sendAT(SFP(TOKEN_CPIN), SFP(TOKEN_READ));
	if(waitResponse(5000L, SFP(TOKEN_CPIN)) != 0) return 0;
	
	safeCopy(replyBuffer + strlen_P(TOKEN_CPIN) + 1, state);
	waitResponse();

	return strlen(state);
}

size_t SIM808::getImei(char *imei)
{
	SENDARROW;
	sendAT(SF("+GSN"));

	readNextLine(SIMCOMAT_DEFAULT_TIMEOUT);
	copyResponse(imei);

	return waitResponse() ?
		strlen(imei) :
		0;
}

SIM808SignalQualityReport SIM808::getSignalQuality()
{
	SIM808SignalQualityReport report = {99, 99, 1};
	SENDARROW;
	sendAT(SFP(TOKEN_CSQ));
	waitResponse(SFP(TOKEN_CSQ));

	if (strstr_P(replyBuffer, TOKEN_CSQ) == 0) return report;

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

	waitResponse();
	return report;
}

bool SIM808::setSmsMessageFormat(SIM808_SMS_MESSAGE_FORMAT format)
{
	SENDARROW;
	sendAT(SF("+CMGF="), (uint8_t)format);

	return waitResponse() == 0;
}

bool SIM808::sendSms(const char *addr, const char *msg)
{
	if (!setSmsMessageFormat(SIM808_SMS_MESSAGE_FORMAT::TEXT)) return false;

	SENDARROW;
	sendAT(SFP(TOKEN_CMGS), SFP(TOKEN_WRITE), SFP(TOKEN_QUOTE), addr, SFP(TOKEN_QUOTE));

	if (!waitResponse(SF(">")) == 0) return false;

	SENDARROW;
	print(msg);
	print((char)0x1A);

	if(!waitResponse(60000, SFP(TOKEN_CMGS)) == 0) return false;
	if(!waitResponse() == 0) return false;

	return true;
}
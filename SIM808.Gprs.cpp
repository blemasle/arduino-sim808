#include "SIM808.h"

SIM808_COMMAND(SET_BEARER_SETTING, "AT+SAPBR=3,1,\"%s\",\"%s\"");
SIM808_COMMAND(GPRS_START_TASK, "AT+CSTT=\"%s\"");
SIM808_COMMAND(GET_NETWORK_REGISTRATION, "AT+CGREG?");

const char SIM808_COMMAND_STRING_PARAMETER[] PROGMEM = ",\"%s\"";

bool SIM808::setBearerSetting(const char* parameter, const char* value)
{
	SENDARROW;	
	_output.verbose(PSTRPTR(SIM808_COMMAND_SET_BEARER_SETTING), parameter, value); //TODO : "%S" format which act like %s but from flash => parameter from flash
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
	_output.verbose(SIM808_COMMAND_GPRS_START_TASK, apn);

	if (user) {
		_output.verbose(PSTRPTR(SIM808_COMMAND_STRING_PARAMETER), user);
	}

	if (password) {
		_output.verbose(PSTRPTR(SIM808_COMMAND_STRING_PARAMETER), password);
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
	SIM808RegistrationStatus result = { -1, -1 };

	SENDARROW;
	_output.verbose(SIM808_COMMAND_GET_NETWORK_REGISTRATION);

	send();
	readLine(1000);
	
	if (strstr_P(replyBuffer, PSTR("+CGREG")) == 0) return result;

	if (!parseReply(',', (uint8_t)SIM808_REGISTRATION_STATUS_RESPONSE::N, &n) ||
		!parseReply(',', (uint8_t)SIM808_REGISTRATION_STATUS_RESPONSE::STAT, &stat)) return result;

	readLine(1000);
	if (!assertResponse(_ok)) return result;

	result.n = n;
	result.stat = stat;

	return result;
}

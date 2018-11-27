#include "SIM808.h"

AT_COMMAND(SET_BEARER_SETTING_PARAMETER, "+SAPBR=3,1,\"%S\",\"%s\"");
AT_COMMAND(SET_BEARER_SETTING, "+SAPBR=%d,%d");
AT_COMMAND(GPRS_START_TASK, "+CSTT=\"%s\",\"%s\",\"%s\"");
AT_COMMAND(GPRS_ATTACH, "+CGATT=%d");

AT_COMMAND_PARAMETER(BEARER, CONTYPE);
AT_COMMAND_PARAMETER(BEARER, APN);
AT_COMMAND_PARAMETER(BEARER, USER);
AT_COMMAND_PARAMETER(BEARER, PWD);

TOKEN_TEXT(CGATT, "+CGATT");
TOKEN_TEXT(CIPSHUT, "+CIPSHUT");
TOKEN_TEXT(CSTT, "+CSTT");
TOKEN_TEXT(CIICR, "+CIICR");
TOKEN_TEXT(SHUT_OK, "SHUT OK");
TOKEN_TEXT(CGREG, "+CGREG");

bool SIM808::setBearerSetting(Sim808ConstStr parameter, const char *value)
{
	sendFormatAT(SFP(AT_COMMAND_SET_BEARER_SETTING_PARAMETER), parameter, value);
	return waitResponse() == 0;
}

bool SIM808::getGprsPowerState(bool *state)
{
	uint8_t result;

	sendAT(SFP(TOKEN_CGATT), SFP(TOKEN_READ));

	if(waitResponse(10000L, SFP(TOKEN_CGATT)) != 0 ||
		!parseReply(',', 0, &result) ||
		waitResponse())
		return false;

	*state = result;
	return true;
}

bool SIM808::enableGprs(const char *apn)
{
	return enableGprs(apn, NULL, NULL);
}

bool SIM808::enableGprs(const char *apn, const char* user, const char *password)
{
	return (sendAT(SFP(TOKEN_CIPSHUT)), waitResponse(65000L, SFP(TOKEN_SHUT_OK)) == 0) &&					//AT+CIPSHUT
		(sendFormatAT(SFP(AT_COMMAND_GPRS_ATTACH), 1), waitResponse(10000L) == 0) &&						//AT+CGATT=1

		setBearerSetting(SFP(AT_COMMAND_PARAMETER_BEARER_CONTYPE), "GPRS") &&
		setBearerSetting(SFP(AT_COMMAND_PARAMETER_BEARER_APN), apn) &&
		(user == NULL || setBearerSetting(SFP(AT_COMMAND_PARAMETER_BEARER_USER), user)) &&
		(password == NULL || setBearerSetting(SFP(AT_COMMAND_PARAMETER_BEARER_PWD), password)) &&

		(sendFormatAT(SFP(AT_COMMAND_GPRS_START_TASK), apn, user, password), waitResponse() == 0) &&		//AT+CSTT="apn","user","password"
		(sendFormatAT(SFP(AT_COMMAND_SET_BEARER_SETTING), 1, 1), waitResponse(65000L) == 0) &&				//AT+SAPBR=1,1
		(sendAT(SFP(TOKEN_CIICR)), waitResponse(65000L) == 0);												//AT+CIICR
}

bool SIM808::disableGprs()
{
	return (sendAT(SFP(TOKEN_CIPSHUT)), waitResponse(65000L, SFP(TOKEN_SHUT_OK)) == 0) &&					//AT+CIPSHUT
		(sendFormatAT(SFP(AT_COMMAND_SET_BEARER_SETTING), 0, 1), waitResponse(65000L) == 0) &&				//AT+SAPBR=0,1
		(sendFormatAT(SFP(AT_COMMAND_GPRS_ATTACH), 0), waitResponse(10000L) == 0);							//AT+CGATT=0
}

SIM808RegistrationStatus SIM808::getNetworkRegistrationStatus()
{
	uint8_t n;
	uint8_t stat;
	SIM808RegistrationStatus result = { -1, SIM808_NETWORK_REGISTRATION_STATE::ERROR };

	sendAT(SFP(TOKEN_CGREG), SFP(TOKEN_READ));
	
	if(waitResponse(SFP(TOKEN_CGREG)) != 0 ||
		!parseReply(',', (uint8_t)SIM808_REGISTRATION_STATUS_RESPONSE::N, &n) ||
		!parseReply(',', (uint8_t)SIM808_REGISTRATION_STATUS_RESPONSE::STAT, &stat) ||
		waitResponse() != 0)
		return result;

	result.n = n;
	result.stat = (SIM808_NETWORK_REGISTRATION_STATE)stat;

	return result;
}

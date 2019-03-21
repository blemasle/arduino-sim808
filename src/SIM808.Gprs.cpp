#include "SIM808.h"

AT_COMMAND(SET_BEARER_SETTING_PARAMETER, "+SAPBR=3,1,\"%S\",\"%s\"");
AT_COMMAND(SET_BEARER_SETTING, "+SAPBR=%d,%d");
AT_COMMAND(GPRS_ATTACH, "+CGATT=%d");

AT_COMMAND_PARAMETER(BEARER, CONTYPE);
AT_COMMAND_PARAMETER(BEARER, APN);
AT_COMMAND_PARAMETER(BEARER, USER);
AT_COMMAND_PARAMETER(BEARER, PWD);

TOKEN_TEXT(GPRS, "GPRS");
TOKEN_TEXT(CGATT, "+CGATT");
TOKEN_TEXT(CIPSHUT, "+CIPSHUT");
TOKEN_TEXT(SHUT_OK, "SHUT OK");
TOKEN_TEXT(CGREG, "+CGREG");

bool SIM808::setBearerSetting(ATConstStr parameter, const char* value)
{
	sendFormatAT(TO_F(AT_COMMAND_SET_BEARER_SETTING_PARAMETER), parameter, value);
	return waitResponse() == 0;
}

bool SIM808::getGprsPowerState(bool *state)
{
	uint8_t result;

	sendAT(TO_F(TOKEN_CGATT), TO_F(TOKEN_READ));

	if(waitResponse(10000L, TO_F(TOKEN_CGATT)) != 0 ||
		!parseReply(',', 0, &result) ||
		waitResponse())
		return false;

	*state = result;
	return true;
}

bool SIM808::enableGprs(const char *apn, const char* user, const char *password)
{
	char gprsToken[5];
	strcpy_P(gprsToken, TOKEN_GPRS);

	return 
		(sendAT(TO_F(TOKEN_CIPSHUT)), waitResponse(65000L, TO_F(TOKEN_SHUT_OK)) == 0) &&					//AT+CIPSHUT
		(sendFormatAT(TO_F(AT_COMMAND_GPRS_ATTACH), 1), waitResponse(10000L) == 0) &&						//AT+CGATT=1

		(setBearerSetting(TO_F(AT_COMMAND_PARAMETER_BEARER_CONTYPE), gprsToken)) &&							//AT+SAPBR=3,1,"CONTYPE","GPRS"

		(setBearerSetting(TO_F(AT_COMMAND_PARAMETER_BEARER_APN), apn)) &&									//AT+SAPBR=3,1,"APN","xxx"
		(user == NULL || setBearerSetting(TO_F(AT_COMMAND_PARAMETER_BEARER_USER), user)) &&					//AT+SAPBR=3,1,"USER","xxx"
		(password == NULL || setBearerSetting(TO_F(AT_COMMAND_PARAMETER_BEARER_PWD), password)) &&			//AT+SAPBR=3,1,"PWD","xxx"

		(sendFormatAT(TO_F(AT_COMMAND_SET_BEARER_SETTING), 1, 1), waitResponse(65000L) == 0);				//AT+SAPBR=1,1
}

bool SIM808::disableGprs()
{
	return 
		(sendFormatAT(TO_F(AT_COMMAND_SET_BEARER_SETTING), 0, 1), waitResponse(65000L) != -1) &&			//AT+SAPBR=0,1
		(sendAT(TO_F(TOKEN_CIPSHUT)), waitResponse(65000L, TO_F(TOKEN_SHUT_OK)) == 0) &&					//AT+CIPSHUT
		(sendFormatAT(TO_F(AT_COMMAND_GPRS_ATTACH), 0), waitResponse(10000L) == 0);							//AT+CGATT=0
}

SIM808NetworkRegistrationState SIM808::getNetworkRegistrationStatus()
{
	uint8_t stat;
	sendAT(TO_F(TOKEN_CGREG), TO_F(TOKEN_READ));
	
	if(waitResponse(TO_F(TOKEN_CGREG)) != 0 ||
		!parseReply(',', (uint8_t)SIM808RegistrationStatusResponse::STAT, &stat) ||
		waitResponse() != 0)
		return SIM808NetworkRegistrationState::ERROR;

	return (SIM808NetworkRegistrationState)stat;
}

#include "SIM808.h"

AT_COMMAND(SET_BEARER_SETTING, "+SAPBR=%d,%d");
AT_COMMAND(GPRS_START_TASK, "+CSTT=\"%s\",\"%s\",\"%s\"");
AT_COMMAND(GPRS_ATTACH, "+CGATT=%d");

TOKEN_TEXT(CGATT, "+CGATT");
TOKEN_TEXT(CIPSHUT, "+CIPSHUT");
TOKEN_TEXT(SHUT_OK, "SHUT OK");
TOKEN_TEXT(CGREG, "+CGREG");

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

bool SIM808::enableGprs(const char *apn, const char* user = NULL, const char *password = NULL)
{
	return 
		(sendAT(TO_F(TOKEN_CIPSHUT)), waitResponse(65000L, TO_F(TOKEN_SHUT_OK)) == 0) &&					//AT+CIPSHUT
		(sendFormatAT(TO_F(AT_COMMAND_GPRS_ATTACH), 1), waitResponse(10000L) == 0) &&						//AT+CGATT=1

		(sendFormatAT(TO_F(AT_COMMAND_GPRS_START_TASK), apn, user, password), waitResponse() == 0) &&		//AT+CSTT="apn","user","password"
		(sendFormatAT(TO_F(AT_COMMAND_SET_BEARER_SETTING), 1, 1), waitResponse(65000L) == 0);				//AT+SAPBR=1,1
}

bool SIM808::disableGprs()
{
	return 
		(sendAT(TO_F(TOKEN_CIPSHUT)), waitResponse(65000L, TO_F(TOKEN_SHUT_OK)) == 0) &&					//AT+CIPSHUT
		(sendFormatAT(TO_F(AT_COMMAND_SET_BEARER_SETTING), 0, 1), waitResponse(65000L) != -1) &&			//AT+SAPBR=0,1
		(sendFormatAT(TO_F(AT_COMMAND_GPRS_ATTACH), 0), waitResponse(10000L) == 0);							//AT+CGATT=0
}

SIM808_NETWORK_REGISTRATION_STATE SIM808::getNetworkRegistrationStatus()
{
	uint8_t stat;
	sendAT(TO_F(TOKEN_CGREG), TO_F(TOKEN_READ));
	
	if(waitResponse(TO_F(TOKEN_CGREG)) != 0 ||
		!parseReply(',', (uint8_t)SIM808_REGISTRATION_STATUS_RESPONSE::STAT, &stat) ||
		waitResponse() != 0)
		return SIM808_NETWORK_REGISTRATION_STATE::ERROR;

	return (SIM808_NETWORK_REGISTRATION_STATE)stat;
}

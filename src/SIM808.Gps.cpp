#include "SIM808.h"

SIM808_COMMAND(SET_GPS_POWER, "AT+CGNSPWR=%d");
SIM808_COMMAND(GET_GPS_POWER, "AT+CGNSPWR?");
SIM808_COMMAND(GET_GPS_INFO, "AT+CGNSINF");

const char SIM808_COMMAND_GET_GPS_INFO_RESPONSE[] PROGMEM = "+CGNSINF: ";
const char SIM808_COMMAND_GET_GPS_POWER_RESPONSE[] PROGMEM = "+CGNSPWR:";

void shiftLeft(uint8_t shift, char* str) //TODO : shiftLeft can be removed if following todos in this file are done
{
	uint8_t len = strlen(str);

	for (uint8_t i = 0; i < len - shift; i++) {
		str[i] = str[i + shift];
	}

	str[len - shift] = '\0';
}

bool SIM808::enableGps() //TODO : merge enableGps & disableGps
{
	bool currentState = true;
	if (!getGpsPowerState(&currentState) || currentState) return false;

	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_SET_GPS_POWER), 1);

	return sendAssertResponse(_ok);
}

bool SIM808::disableGps()
{
	bool currentState = false;
	if (!getGpsPowerState(&currentState) || !currentState) return false;

	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_SET_GPS_POWER), 0);

	return sendAssertResponse(_ok);
}

bool SIM808::getGpsPosition(char *response)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_GET_GPS_INFO));

	//TODO : read response directly into user supplied response like in readHttpResponse
	//TODO : check for SIM808_COMMAND_GET_GPS_INFO_RESPONSE at the beginning of the response
	if (!sendGetResponse(response)) return false;
	shiftLeft(strlen_P(SIM808_COMMAND_GET_GPS_INFO_RESPONSE), response);

	readLine();
	if (!assertResponse(_ok)) return false;

	return true;
}

void SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, char** result) 
{
	char *pTmp = find(response, ',', (uint8_t)field);
	*result = pTmp;
}

bool SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, uint8_t* result)
{
	if (field < SIM808_GPS_FIELD::ALTITUDE) return false; //using parse as a rough float values truncating

	parse(response, ',', (uint8_t)field, result);
	return true;
}

bool SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, float* result)
{
	if (field != SIM808_GPS_FIELD::COURSE && 
		field != SIM808_GPS_FIELD::LATITUDE &&
		field != SIM808_GPS_FIELD::LONGITUDE &&
		field != SIM808_GPS_FIELD::SPEED) return false;

	parse(response, ',', (uint8_t)field, result);
	return true;
}

__attribute__((__optimize__("O2")))
SIM808_GPS_STATUS SIM808::getGpsStatus(char * response)
{	
	SIM808_GPS_STATUS result = SIM808_GPS_STATUS::NO_FIX;

	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_GET_GPS_INFO));

	if (!sendGetResponse(NULL)) return SIM808_GPS_STATUS::FAIL;
	shiftLeft(strlen_P(SIM808_COMMAND_GET_GPS_INFO_RESPONSE), replyBuffer); //TODO : use constant + strlen_P instead of shiftLeft

	if (replyBuffer[0] == '0') result = SIM808_GPS_STATUS::OFF;
	if (replyBuffer[2] == '1')
	{
		uint8_t satellitesUsed;
		result = getGpsField(replyBuffer, SIM808_GPS_FIELD::GNSS_USED, &satellitesUsed) && satellitesUsed > GPS_ACCURATE_FIX_MIN_SATELLITES ?
			SIM808_GPS_STATUS::ACCURATE_FIX :
			SIM808_GPS_STATUS::FIX;

		copyResponse(response);
	}

	readLine();
	if (!assertResponse(_ok)) return SIM808_GPS_STATUS::FAIL;

	return result;
}

bool SIM808::getGpsPowerState(bool *state)
{
	uint8_t result;
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_GET_GPS_POWER));

	send();
	readLine();
	if(strstr_P(replyBuffer, SIM808_COMMAND_GET_GPS_POWER_RESPONSE) == 0) return false;

	if (!parseReply(',', 0, &result)) return false;

	*state = result;

	readLine();
	return assertResponse(_ok);
}

#include "SIM808.h"

SIM808_COMMAND(SET_GPS_POWER, "AT+CGNSPWR=%d");
SIM808_COMMAND(GET_GPS_POWER, "AT+CGNSPWR?");
SIM808_COMMAND(GET_GPS_INFO, "AT+CGNSINF");

const char SIM808_COMMAND_GET_GPS_INFO_RESPONSE[] PROGMEM = "+CGNSINF:";
const char SIM808_COMMAND_GET_GPS_POWER_RESPONSE[] PROGMEM = "+CGNSPWR:";

void shiftLeft(uint8_t i, char* str)
{
	uint8_t j = i;
	char* p = str;

	while (j--) *p = *(p + sizeof(char));	
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
	_output.verbose(SIM808_COMMAND_GET_GPS_INFO);

	if (!sendGetResponse(response)) return false;
	shiftLeft(strlen_P(SIM808_COMMAND_GET_GPS_INFO_RESPONSE), response);

	readLine();
	if (!assertResponse(_ok)) return false;

	return true;
}

void SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, char* result) 
{
	result = find(response, ',', (uint8_t)field);
}

bool SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, uint8_t* result)
{
	if (field != SIM808_GPS_FIELD::GNSS_USED &&
		field != SIM808_GPS_FIELD::GPS_IN_VIEW) return false;

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

SIM808_GPS_STATUS SIM808::getGpsStatus()
{	
	SIM808_GPS_STATUS result = SIM808_GPS_STATUS::NO_FIX;

	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_GET_GPS_INFO));

	if (!sendGetResponse(NULL)) return SIM808_GPS_STATUS::FAIL;
	shiftLeft(strlen_P(SIM808_COMMAND_GET_GPS_INFO_RESPONSE), replyBuffer);

	if (replyBuffer[0] == '0') result = SIM808_GPS_STATUS::OFF;
	if (replyBuffer[2] == '1')
	{
		uint8_t satellitesUsed;
		result = getGpsField(replyBuffer, SIM808_GPS_FIELD::GNSS_USED, &satellitesUsed) && satellitesUsed > 4 ?
			SIM808_GPS_STATUS::ACCURATE_FIX :
			SIM808_GPS_STATUS::FIX;
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

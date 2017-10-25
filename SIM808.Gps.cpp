#include "SIM808.h"

void shiftLeft(uint8_t i, char* str)
{
	uint8_t j = i;
	char* p = str;

	while (j--) *p = *(p + sizeof(char));	
}

bool SIM808::enableGps()
{
	bool currentState = false;
	if (!getGpsPowerState(&currentState) || currentState) return false;

	SENDARROW;
	print("AT+CGNSPWR=1");

	return sendAssertResponse(_ok);
}

bool SIM808::disableGps()
{
	bool currentState = false;
	if (!getGpsPowerState(&currentState) || !currentState) return false;

	SENDARROW;
	print("AT+CGNSPWR=0");

	return sendAssertResponse(_ok);
}

bool SIM808::getGpsPosition(char *response)
{
	SENDARROW;
	print("AT+CGNSINF");

	if (!sendGetResponse(response)) return false;
	shiftLeft(strlen_PF(F("+CGNSINF:")), response);

	readLine(1000);
	if (!assertResponse(_ok)) return false;

	return true;
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
	print("AT+CGNSINF");

	if (!sendGetResponse(NULL)) return SIM808_GPS_STATUS::FAIL;
	shiftLeft(strlen_PF(F("+CGNSINF:")), replyBuffer);

	if (replyBuffer[0] == '0') result = SIM808_GPS_STATUS::OFF;
	if (replyBuffer[2] == '1')
	{
		uint8_t satellitesUsed;
		result = getGpsField(replyBuffer, SIM808_GPS_FIELD::GNSS_USED, &satellitesUsed) && satellitesUsed > 4 ?
			SIM808_GPS_STATUS::ACCURATE_FIX :
			SIM808_GPS_STATUS::FIX;
	}

	readLine(1000);
	if (!assertResponse(_ok)) return SIM808_GPS_STATUS::FAIL;

	return result;
}

bool SIM808::getGpsPowerState(bool *state)
{
	uint8_t result;
	SENDARROW;
	print("AT+CGNSPWR?");

	send();
	readLine(1000);
	if (strstr(replyBuffer, "+CGNSPWR") == 0) return false;

	if (!parseReply(',', 0, &result)) return false;

	*state = result;

	readLine(1000);
	return assertResponse(_ok);
}

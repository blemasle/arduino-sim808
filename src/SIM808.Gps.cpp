#include "SIM808.h"

TOKEN_TEXT(GPS_POWER, "+CGNSPWR");
TOKEN_TEXT(GPS_INFO, "+CGNSINF");

bool SIM808::powerOnOffGps(bool power)
{
	bool currentState;
	if(!getGpsPowerState(&currentState) || (currentState == power)) return false;

	sendAT(TO_F(TOKEN_GPS_POWER), TO_F(TOKEN_WRITE), (uint8_t)power);
	return waitResponse() == 0;
}

bool SIM808::getGpsPosition(char *response, size_t responseSize)
{
	sendAT(TO_F(TOKEN_GPS_INFO));

	if(waitResponse(TO_F(TOKEN_GPS_INFO)) != 0)
		return false;

	// GPSINF response might be too long for the reply buffer
	copyCurrentLine(response, responseSize, strlen_P(TOKEN_GPS_INFO) + 2);
}

void SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, char** result) 
{
	char *pTmp = find(response, ',', (uint8_t)field);
	*result = pTmp;
}

bool SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, uint16_t* result)
{
	if (field < SIM808_GPS_FIELD::SPEED) return false;

	parse(response, ',', (uint8_t)field, result);
	return true;
}

bool SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, float* result)
{
	if (field != SIM808_GPS_FIELD::COURSE && 
		field != SIM808_GPS_FIELD::LATITUDE &&
		field != SIM808_GPS_FIELD::LONGITUDE &&
		field != SIM808_GPS_FIELD::ALTITUDE &&
		field != SIM808_GPS_FIELD::SPEED) return false;

	parse(response, ',', (uint8_t)field, result);
	return true;
}

SIM808_GPS_STATUS SIM808::getGpsStatus(char * response, size_t responseSize, uint8_t minSatellitesForAccurateFix)
{	
	SIM808_GPS_STATUS result = SIM808_GPS_STATUS::NO_FIX;

	sendAT(TO_F(TOKEN_GPS_INFO));

	if(waitResponse(TO_F(TOKEN_GPS_INFO)) != 0)
		return SIM808_GPS_STATUS::FAIL;

	uint16_t shift = strlen_P(TOKEN_GPS_INFO) + 2;

	if(replyBuffer[shift] == '0') result = SIM808_GPS_STATUS::OFF;
	if(replyBuffer[shift + 2] == '1') // fix acquired
	{
		uint16_t satellitesUsed;
		getGpsField(replyBuffer, SIM808_GPS_FIELD::GNSS_USED, &satellitesUsed);

		result = satellitesUsed > minSatellitesForAccurateFix ?
			SIM808_GPS_STATUS::ACCURATE_FIX :
			SIM808_GPS_STATUS::FIX;

		copyCurrentLine(response, responseSize, shift);
	}

	if(waitResponse() != 0) return SIM808_GPS_STATUS::FAIL;

	return result;
}

bool SIM808::getGpsPowerState(bool *state)
{
	uint8_t result;

	sendAT(TO_F(TOKEN_GPS_POWER), TO_F(TOKEN_READ));

	if(waitResponse(10000L, TO_F(TOKEN_GPS_POWER)) != 0 ||
		!parseReply(',', 0, &result) ||
		waitResponse())
		return false;

	*state = result;
	return true;
}

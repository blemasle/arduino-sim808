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

void SIM808::getGpsField(const char* response, SIM808GpsField field, char** result) 
{
	char *pTmp = find(response, ',', (uint8_t)field);
	*result = pTmp;
}

bool SIM808::getGpsField(const char* response, SIM808GpsField field, uint16_t* result)
{
	if (field < SIM808GpsField::Speed) return false;

	parse(response, ',', (uint8_t)field, result);
	return true;
}

bool SIM808::getGpsField(const char* response, SIM808GpsField field, float* result)
{
	if (field != SIM808GpsField::Course && 
		field != SIM808GpsField::Latitude &&
		field != SIM808GpsField::Longitude &&
		field != SIM808GpsField::Altitude &&
		field != SIM808GpsField::Speed) return false;

	parse(response, ',', (uint8_t)field, result);
	return true;
}

SIM808GpsStatus SIM808::getGpsStatus(char * response, size_t responseSize, uint8_t minSatellitesForAccurateFix)
{	
	SIM808GpsStatus result = SIM808GpsStatus::NoFix;

	sendAT(TO_F(TOKEN_GPS_INFO));

	if(waitResponse(TO_F(TOKEN_GPS_INFO)) != 0)
		return SIM808GpsStatus::Fail;

	uint16_t shift = strlen_P(TOKEN_GPS_INFO) + 2;

	if(replyBuffer[shift] == '0') result = SIM808GpsStatus::Off;
	if(replyBuffer[shift + 2] == '1') // fix acquired
	{
		uint16_t satellitesUsed;
		getGpsField(replyBuffer, SIM808GpsField::GnssUsed, &satellitesUsed);

		result = satellitesUsed > minSatellitesForAccurateFix ?
			SIM808GpsStatus::AccurateFix :
			SIM808GpsStatus::Fix;

		copyCurrentLine(response, responseSize, shift);
	}

	if(waitResponse() != 0) return SIM808GpsStatus::Fail;

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

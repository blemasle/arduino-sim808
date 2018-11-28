#include "SIM808.h"

TOKEN_TEXT(GPS_POWER, "+CGNSPWR");
TOKEN_TEXT(GPS_INFO, "+CGNSINF");

void shiftLeft(uint8_t shift, char* str) //TODO : shiftLeft can be removed if following todos in this file are done
{
	uint8_t len = strlen(str);

	for (uint8_t i = 0; i < len - shift; i++) {
		str[i] = str[i + shift];
	}

	str[len - shift] = '\0';
}

bool SIM808::powerOnOffGps(bool power)
{
	bool currentState;
	if(!getGpsPowerState(&currentState) || (currentState == power)) return false;

	sendAT(SFP(TOKEN_GPS_POWER), SFP(TOKEN_WRITE), (uint8_t)power);
	return  waitResponse() == 0;
}

bool SIM808::getGpsPosition(char *response)
{
	sendAT(SFP(TOKEN_GPS_INFO));

	if(waitResponse(SFP(TOKEN_GPS_INFO)) != 0)
		return false;

	// GPSINF response might be too long for the reply buffer
	copyResponse(response, strlen_P(TOKEN_GPS_INFO) + 2);
}

void SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, char** result) 
{
	char *pTmp = find(response, ',', (uint8_t)field);
	*result = pTmp;
}

//TODO : change to int16_t : altitude can be negative, and course > 255
bool SIM808::getGpsField(const char* response, SIM808_GPS_FIELD field, uint8_t* result)
{
	// it is possible to get float fields as int. They will be truncated by parse
	if (field < SIM808_GPS_FIELD::ALTITUDE) return false;

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

	sendAT(SFP(TOKEN_GPS_INFO));

	if(waitResponse(SFP(TOKEN_GPS_INFO)) != 0)
		return SIM808_GPS_STATUS::FAIL;

	uint16_t shift = strlen_P(TOKEN_GPS_INFO) + 2;

	if(replyBuffer[shift] == '0') result = SIM808_GPS_STATUS::OFF;
	if(replyBuffer[shift + 1] == '1') // fix acquired
	{
		uint8_t satellitesUsed;
		result = getGpsField(replyBuffer, SIM808_GPS_FIELD::GNSS_USED, &satellitesUsed) && satellitesUsed > GPS_ACCURATE_FIX_MIN_SATELLITES ?
			SIM808_GPS_STATUS::ACCURATE_FIX :
			SIM808_GPS_STATUS::FIX;

		copyResponse(response, shift);
	}

	if(waitResponse() == 0) return SIM808_GPS_STATUS::FAIL;

	return result;
}

bool SIM808::getGpsPowerState(bool *state)
{
	uint8_t result;

	sendAT(SFP(TOKEN_GPS_POWER), SFP(TOKEN_READ));

	if(waitResponse(10000L, SFP(TOKEN_GPS_POWER)) != 0 ||
		!parseReply(',', 0, &result) ||
		waitResponse())
		return false;

	*state = result;
	return true;
}

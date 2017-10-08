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

SIM808_GPS_STATUS SIM808::getGpsStatus()
{
	char buffer[125];
	if (!getGpsPosition(buffer)) return SIM808_GPS_STATUS::FAIL;

	if (buffer[0] == '0') return SIM808_GPS_STATUS::OFF;
	if (buffer[2] == '1')
	{
		//TODO : based on satellites count, ACCURATE_FIX if > 4
		return SIM808_GPS_STATUS::FIX;
	}

	return SIM808_GPS_STATUS::NO_FIX;
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

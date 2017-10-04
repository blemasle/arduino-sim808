#include "SIM808.h"

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

	sendGetResponse(response);
	return true;
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

#include "SIM808.h"

SIM808::SIM808(int8_t resetPin)
{
	_resetPin = resetPin;
	_ok = "OK";
}

SIM808::~SIM808() { }

#pragma region Public functions

void SIM808::init()
{
	reset();
	waitForReady();
	delay(3000);

	setEcho(SIM808_BOOL::OFF);
	setEcho(SIM808_BOOL::OFF);
}

void SIM808::reset()
{
	pinMode(_resetPin, OUTPUT);
	digitalWrite(_resetPin, HIGH);
	delay(10);
	digitalWrite(_resetPin, LOW);
	delay(200);

	digitalWrite(_resetPin, HIGH);
}

void SIM808::waitForReady()
{
	do
	{
		PRINTLN(F("Waiting for echo..."));
	} while (!sendAssertResponse("AT", "AT"));

	do
	{
		PRINTLN(F("Waiting for RDY..."));
		readLine(1000);
	} while (!assertResponse("RDY"));

}

bool SIM808::setEcho(SIM808_BOOL mode)
{
	SENDARROW;
	print("ATE");
	print(mode);

	return sendAssertResponse(_ok);
}

size_t SIM808::sendCommand(const char *cmd, char *response)
{
	flushInput();
	print(cmd);
	sendGetResponse(response);
	readLine(1000);

	return strlen(response);
}

#pragma endregion



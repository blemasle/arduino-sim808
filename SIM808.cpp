#include "SIM808.h"

SIM808::SIM808(uint8_t resetPin, uint8_t pwrKeyPin, uint8_t statusPin)
{
	_resetPin = resetPin;
	_pwrKeyPin = pwrKeyPin;
	_statusPin = statusPin;
	_ok = "OK";
}

SIM808::~SIM808() { }

#pragma region Public functions

void SIM808::init()
{
	pinMode(_resetPin, OUTPUT);
	pinMode(_pwrKeyPin, OUTPUT);
	pinMode(_statusPin, INPUT);

	reset();
	waitForReady();
	delay(3000);

	setEcho(SIM808_BOOL::OFF);
	setEcho(SIM808_BOOL::OFF);
}

void SIM808::reset()
{
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



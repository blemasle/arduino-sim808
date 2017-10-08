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

	setEcho(false);
	setEcho(false);
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

bool SIM808::setEcho(bool state)
{
	SENDARROW;
	print("ATE");
	print(state ? 1 : 0);

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



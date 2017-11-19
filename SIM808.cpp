#include "SIM808.h"

SIM808::SIM808(uint8_t resetPin, uint8_t pwrKeyPin, uint8_t statusPin)
{
	_resetPin = resetPin;
	_pwrKeyPin = pwrKeyPin;
	_statusPin = statusPin;
	_ok = "OK";

	pinMode(_resetPin, OUTPUT);
	pinMode(_pwrKeyPin, OUTPUT);
	pinMode(_statusPin, INPUT);
	
	digitalWrite(_pwrKeyPin, HIGH);
	digitalWrite(_resetPin, HIGH);
}

SIM808::~SIM808() { }

#pragma region Public functions

void SIM808::init()
{
	reset();
	waitForReady();
	delay(3000);

	setEcho(SIM808_ECHO::OFF);
	setEcho(SIM808_ECHO::OFF);
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
		SIM808_PRINTLN(F("Waiting for echo..."));
	} while (!sendAssertResponse("AT", "AT"));

	do
	{
		SIM808_PRINTLN(F("Waiting for RDY..."));
		readLine(1000);
	} while (!assertResponse("RDY"));

}

bool SIM808::setEcho(SIM808_ECHO mode)
{
	SENDARROW;
	print("ATE");
	print((uint8_t)mode);

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



#include "SIM808.h"

TOKEN(RDY);

SIM808::SIM808(uint8_t resetPin, uint8_t pwrKeyPin, uint8_t statusPin)
{
	_resetPin = resetPin;
	_pwrKeyPin = pwrKeyPin;
	_statusPin = statusPin;

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
	SIM808_PRINT_SIMPLE_P("Init...");

	reset();
	waitForReady();
	delay(1500);

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
		SIM808_PRINT_SIMPLE_P("Waiting for echo...");
		sendAT(SF(""));
	// Despite official documentation, we can get an "AT" back without a "RDY" first.
	} while (waitResponse(SFP(TOKEN_AT)) != 0);

	// we got AT, waiting for RDY
	while (waitResponse(SFP(TOKEN_RDY)) != 0);
}

bool SIM808::setEcho(SIM808_ECHO mode)
{
	sendAT(SF("E"), (uint8_t)mode);

	return waitResponse() == 0;
}

size_t SIM808::sendCommand(const char *cmd, char *response)
{
	sendAT(cmd);
	
	if(waitResponse() != 0) return 0;
	return copyCurrentLine(response);
}

#pragma endregion



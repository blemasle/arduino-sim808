#include "SIM808.h"

TOKEN_TEXT(CBC, "+CBC");
TOKEN_TEXT(CFUN, "+CFUN");

bool SIM808::powered()
{
	if(_statusPin == SIM808_UNAVAILABLE_PIN) {
		sendAT();
		return waitResponse(SIMCOMAT_DEFAULT_TIMEOUT) != -1;
	}
	
	return digitalRead(_statusPin) == HIGH;
}

bool SIM808::powerOnOff(bool power)
{
	if (_pwrKeyPin == SIM808_UNAVAILABLE_PIN) return false;

	bool currentlyPowered = powered();
	if (currentlyPowered == power) return false;
	
	SIM808_PRINT_P("powerOnOff: %t", power);

	digitalWrite(_pwrKeyPin, LOW);
	delay(2000);
	digitalWrite(_pwrKeyPin, HIGH);

	uint16_t timeout = 2000;
	do {
		delay(150);
		timeout -= 150;
		currentlyPowered = powered();
	} while(currentlyPowered != power && timeout > 0);

	return currentlyPowered == power;
}

SIM808ChargingStatus SIM808::getChargingState()
{
	uint8_t state;
	uint8_t level;
	uint16_t voltage;

	sendAT(TO_F(TOKEN_CBC));

	if (waitResponse(TO_F(TOKEN_CBC)) == 0 &&
		parseReply(',', (uint8_t)SIM808BatteryChargeField::BCS, &state) &&
		parseReply(',', (uint8_t)SIM808BatteryChargeField::BCL, &level) &&
		parseReply(',', (uint16_t)SIM808BatteryChargeField::VOLTAGE, &voltage) &&
		waitResponse() == 0)
		return { (SIM808ChargingState)state, level, voltage };
			
	return { SIM808ChargingState::ERROR, 0, 0 };
}

SIM808PhoneFunctionality SIM808::getPhoneFunctionality()
{
	uint8_t state;

	sendAT(TO_F(TOKEN_CFUN), TO_F(TOKEN_READ));

	if (waitResponse(10000L, TO_F(TOKEN_CFUN)) == 0 &&
		parseReply(',', 0, &state) &&
		waitResponse() == 0)
		return (SIM808PhoneFunctionality)state;
	
	return SIM808PhoneFunctionality::FAIL;
}

bool SIM808::setPhoneFunctionality(SIM808PhoneFunctionality fun)
{
	sendAT(TO_F(TOKEN_CFUN), TO_F(TOKEN_WRITE), (uint8_t)fun);

	return waitResponse(10000L) == 0;
}

bool SIM808::setSlowClock(SIM808SlowClock mode)
{
	sendAT(S_F("+CSCLK"), TO_F(TOKEN_WRITE), (uint8_t)mode);

	return waitResponse() == 0;
}


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
		parseReply(',', (uint8_t)SIM808_BATTERY_CHARGE_FIELD::BCS, &state) &&
		parseReply(',', (uint8_t)SIM808_BATTERY_CHARGE_FIELD::BCL, &level) &&
		parseReply(',', (uint16_t)SIM808_BATTERY_CHARGE_FIELD::VOLTAGE, &voltage) &&
		waitResponse() == 0)
		return { (SIM808_CHARGING_STATE)state, level, voltage };
			
	return { SIM808_CHARGING_STATE::ERROR, 0, 0 };
}

SIM808_PHONE_FUNCTIONALITY SIM808::getPhoneFunctionality()
{
	uint8_t state;

	sendAT(TO_F(TOKEN_CFUN), TO_F(TOKEN_READ));

	if (waitResponse(10000L, TO_F(TOKEN_CFUN)) == 0 &&
		parseReply(',', 0, &state) &&
		waitResponse() == 0)
		return (SIM808_PHONE_FUNCTIONALITY)state;
	
	return SIM808_PHONE_FUNCTIONALITY::FAIL;
}

bool SIM808::setPhoneFunctionality(SIM808_PHONE_FUNCTIONALITY fun)
{
	sendAT(TO_F(TOKEN_CFUN), TO_F(TOKEN_WRITE), (uint8_t)fun);

	return waitResponse(10000L) == 0;
}

bool SIM808::setSlowClock(SIM808_SLOW_CLOCK mode)
{
	sendAT(S_F("+CSCLK"), TO_F(TOKEN_WRITE), (uint8_t)mode);

	return waitResponse() == 0;
}


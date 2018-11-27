#include "SIM808.h"

TOKEN_TEXT(CBC, "+CBC");
TOKEN_TEXT(CFUN, "+CFUN");

bool SIM808::powered()
{
	return digitalRead(_statusPin) == HIGH;
}

bool SIM808::powerOnOff(bool power)
{
	if (powered() == power) return false;

	SIM808_PRINT_P("powerOnOff: %t", power);

	if(power) {
		digitalWrite(_pwrKeyPin, LOW);
		delay(2000);
		digitalWrite(_pwrKeyPin, HIGH);
		delay(150);
	} else sendAT(SF("+CPOWD=1"));
	
	return true;
}

SIM808ChargingStatus SIM808::getChargingState()
{
	uint8_t state;
	uint8_t level;
	uint16_t voltage;

	SENDARROW;
	sendAT(SFP(TOKEN_CBC));

	if (waitResponse(SFP(TOKEN_CBC)) == 0 &&
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

	SENDARROW;
	sendAT(SFP(TOKEN_CFUN), SFP(TOKEN_READ));

	if (waitResponse(10000L, SFP(TOKEN_CFUN)) == 0 &&
		parseReply(',', 0, &state) &&
		waitResponse() == 0)
		return (SIM808_PHONE_FUNCTIONALITY)state;
	
	return SIM808_PHONE_FUNCTIONALITY::FAIL;
}

bool SIM808::setPhoneFunctionality(SIM808_PHONE_FUNCTIONALITY fun)
{
	SENDARROW;
	sendAT(SFP(TOKEN_CFUN), SFP(TOKEN_WRITE), (uint8_t)fun);

	return waitResponse(10000L) == 0;
}

bool SIM808::setSlowClock(SIM808_SLOW_CLOCK mode)
{
	SENDARROW;
	sendAT(SF("+CSCLK"), SFP(TOKEN_WRITE), (uint8_t)mode);

	return waitResponse() == 0;
}


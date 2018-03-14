#include "SIM808.h"

SIM808_COMMAND(GET_CHARGING_STATE, "AT+CBC");
SIM808_COMMAND(SET_PHONE_FUNCTIONNALITY, "AT+CFUN=%d");
SIM808_COMMAND(SET_SLOW_CLOCK, "AT+CSCLK=%d");

const char SIM808_COMMAND_GET_CHARGING_STATE_RESPONSE[] PROGMEM = "+CBC:";


bool SIM808::powered()
{
	return digitalRead(_statusPin) == HIGH;
}

bool SIM808::powerOnOff(bool power)
{
	if (powered() == power) return false;

	SIM808_PRINT_P("powerOnOff: %t", power);

	digitalWrite(_pwrKeyPin, LOW);
	delay(2000);
	digitalWrite(_pwrKeyPin, HIGH);
	delay(150);

	return true;
}

SIM808ChargingStatus SIM808::getChargingState()
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_GET_CHARGING_STATE));
	send();

	readLine();
	if (strstr_P(replyBuffer, SIM808_COMMAND_GET_CHARGING_STATE_RESPONSE) == 0) return { SIM808_CHARGING_STATE::ERROR, 0, 0 };

	uint8_t state;
	uint8_t level;
	uint16_t voltage;
	parseReply(',', (uint8_t)SIM808_BATTERY_CHARGE_FIELD::BCS, &state);
	parseReply(',', (uint8_t)SIM808_BATTERY_CHARGE_FIELD::BCL, &level);
	parseReply(',', (uint16_t)SIM808_BATTERY_CHARGE_FIELD::VOLTAGE, &voltage);

	readLine();
	if (!assertResponse(_ok)) return { SIM808_CHARGING_STATE::ERROR, 0 };

	return { (SIM808_CHARGING_STATE)state, level, voltage };
}

bool SIM808::setPhoneFunctionality(SIM808_PHONE_FUNCTIONALITY fun)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_SET_PHONE_FUNCTIONNALITY), fun);

	//TODO : refactor send etc to allow for such things
	send();
	//ditching URC given back by this command
	readLine(10000);
	if (assertResponse(_ok)) return true;

	while (readLine(1000)) {
		if (assertResponse(_ok)) return true;

	}
	return false;
}

bool SIM808::setSlowClock(SIM808_SLOW_CLOCK mode)
{
	SENDARROW;
	_output.verbose(PSTRPTR(SIM808_COMMAND_SET_SLOW_CLOCK), mode);

	return sendAssertResponse(_ok, 1000);
}


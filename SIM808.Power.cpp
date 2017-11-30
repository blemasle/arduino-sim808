#include "SIM808.h"

bool SIM808::powered()
{
	return digitalRead(_statusPin) == HIGH;
}

void SIM808::powerOnOff(bool power)
{
	if (powered() == power) return;

	SIM808_PRINT("powerOnOff :");
	SIM808_PRINTLN(power);

	digitalWrite(_pwrKeyPin, LOW);
	delay(2000);
	digitalWrite(_pwrKeyPin, HIGH);
	delay(150);
}

SIM808ChargingStatus SIM808::getChargingState()
{
	SENDARROW;
	print("AT+CBC");
	send();

	readLine(1000);
	if (strstr(replyBuffer, "+CBC") == 0) return { SIM808_CHARGING_STATE::ERROR, 0, 0 };

	uint8_t state;
	uint8_t level;
	uint16_t voltage;
	parseReply(',', (uint8_t)SIM808_BATTERY_CHARGE_FIELD::BCS, &state);
	parseReply(',', (uint8_t)SIM808_BATTERY_CHARGE_FIELD::BCL, &level);

	readLine(1000);
	if (!assertResponse(_ok)) return { SIM808_CHARGING_STATE::ERROR, 0 };

	return { (SIM808_CHARGING_STATE)state, level };
}

bool SIM808::setPhoneFunctionality(SIM808_PHONE_FUNCTIONALITY fun)
{
	SENDARROW;
	print("AT+CFUN=");
	print((uint8_t)fun);

	return sendAssertResponse(_ok, 10000);
}

bool SIM808::setSlowClock(SIM808_SLOW_CLOCK mode)
{
	SENDARROW;
	print("AT+CSCLK=");
	print((uint8_t)mode);

	return sendAssertResponse(_ok, 1000);
}


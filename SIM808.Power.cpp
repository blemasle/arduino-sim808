#include "SIM808.h"

bool SIM808::powered()
{
	return digitalRead(_statusPin) == HIGH;
}

void SIM808::powerOnOff(bool power)
{
	if (powered() == power) return;

	PRINT("powerOnOff :");
	PRINTLN(power);

	digitalWrite(_pwrKeyPin, LOW);
	delay(2000);
	digitalWrite(_pwrKeyPin, HIGH);
}
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


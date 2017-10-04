#include "SIM808.h"

bool SIM808::powered()
{
	return digitalRead(_pwrKeyPin) != 0;
}

void SIM808::powerOnOff(bool power)
{
	if (powered() == power) return;

	digitalWrite(_pwrKeyPin, HIGH);
	delay(1200);
	digitalWrite(_pwrKeyPin, LOW);
}

bool SIM808::setPhoneFunctionality(SIM808_PHONE_FUNCTIONALITY fun)
{
	SENDARROW;
	print("AT+CFUN=");
	print(fun);

	return sendAssertResponse(_ok, 10000);
}

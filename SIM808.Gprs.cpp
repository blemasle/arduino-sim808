#include "SIM808.h"

bool SIM808::setBearerSetting(const char* parameter, const char* value)
{
	SENDARROW;
	print("AT+SAPBR=3,1,\"");
	print(parameter);
	print("\",\"");
	print(value);
	print('"');

	return sendAssertResponse(_ok);
}

bool SIM808::enableGprs(const char *apn)
{
	return enableGprs(apn, NULL, NULL);
}

bool SIM808::enableGprs(const char *apn, const char* user, const char *password)
{
	bool success = sendAssertResponse("AT+CIPSHUT", "SHUT OK", 65000) &&
		sendAssertResponse("AT+CGATT=1", _ok, 10000) &&
		setBearerSetting("CONTYPE", "GPRS") &&
		setBearerSetting("APN", apn) &&
		(user == NULL || setBearerSetting("USER", user)) &&
		(password == NULL || setBearerSetting("PWD", password));

	if (!success) return false;

	SENDARROW;
	print("AT+CSTT=\"");
	print(apn);
	print('"');

	if (user) {
		print(",\"");
		print(user);
		print('"');
	}

	if (password) {
		print(",\"");
		print(password);
		print('"');
	}

	if (!sendAssertResponse(_ok)) return false;

	return sendAssertResponse("AT+SAPBR=1,1", _ok, 65000) &&
		sendAssertResponse("AT+CIICR", _ok, 65000);

}

bool SIM808::disableGprs()
{
	sendAssertResponse("AT+CIPSHUT", "SHUT OK", 65000);
	sendAssertResponse("AT+SAPBR=0,1", _ok, 65000);
	sendAssertResponse("AT+CGATT=0", _ok, 10000);

	return true;
}

SIM808RegistrationStatus SIM808::getNetworkRegistrationStatus()
{
	uint8_t n;
	uint8_t stat;
	SIM808RegistrationStatus result = { -1, -1 };

	SENDARROW;
	print("AT+CGREG?");

	send();
	readLine(1000);
	if (strstr(replyBuffer, "+CGREG") == 0) return result;

	if (!parseReply(',', (uint8_t)SIM808_REGISTRATION_STATUS_RESPONSE::N, &n) ||
		!parseReply(',', (uint8_t)SIM808_REGISTRATION_STATUS_RESPONSE::STAT, &stat)) return result;

	readLine(1000);
	if (!assertResponse(_ok)) return result;

	PRINT("getNetworkRegistrationStatus : ");
	PRINT(n);
	PRINT(", ");
	PRINTLN(stat);

	result.n = n;
	result.stat = stat;

	return result;
}

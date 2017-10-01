#pragma once

#include "SIMComAT.h"
#include "SIM808_Types.h"

#define BUFFER_SIZE 255

class SIM808 : public SIMComAT
{
private:
	int8_t _resetPin;
	const char* _ok;
	const char* _userAgent;

	int16_t _httpTimeout = 10000;

	void init();
	void waitForReady();	

	bool setupHttpRequest(const char* url);
	bool fireHttpRequest(const SIM808_HTTP_ACTION action, uint16_t *statusCode, size_t *dataSize);
	bool readHttpResponse(char *response, size_t responseSize);
	bool setHttpParameter(const char* parameter, const char* value);
	bool setHttpParameter(const char* parameter, const int8_t value);
	bool setHttpBody(const char* body);
	bool httpInit();
	bool httpEnd();

	bool setBearerSetting(const char* parameter, const char* value);

	bool getGpsPowerState(bool *state);

public:
	SIM808(int8_t resetPin);
	~SIM808();

	void reset();

	size_t sendCommand(const char *cmd, char *response);

	bool setEcho(SIM808_BOOL state);
	bool simUnlock(const char *pin);
	size_t getSimState(char *state);
	size_t getImei(char *imei);

	bool setSmsMessageFormat(SIM808_SMS_MESSAGE_FORMAT format);
	bool sendSms(const char *addr, const char *msg);

	bool enableGprs(const char *apn);
	bool enableGprs(const char *apn, const char* user, const char *password);
	bool disableGprs();
	SIM808RegistrationStatus getNetworkRegistrationStatus();

	bool enableGps();
	bool disableGps();
	bool getGpsPosition(char *response);

	uint16_t httpGet(const char *url, char *response, size_t responseSize);
	uint16_t httpPost(const char *url, const char *contentType, const char *body, char *response, size_t responseSize);	
};


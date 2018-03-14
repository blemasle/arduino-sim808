#pragma once

#include <SIMComAT.h>
#include <SIM808_Types.h>
#include <SIM808_Commands.h>

class SIM808 : public SIMComAT
{
private:
	uint8_t _resetPin;
	uint8_t _statusPin;
	uint8_t _pwrKeyPin;

	const __FlashStringHelper *_ok;
	const char* _userAgent;

	int16_t _httpTimeout = 10000;
	
	void waitForReady();	

	bool setupHttpRequest(const char* url);
	bool fireHttpRequest(const SIM808_HTTP_ACTION action, uint16_t *statusCode, size_t *dataSize);
	bool readHttpResponse(char *response, size_t responseSize);
	bool setHttpParameter(const __FlashStringHelper* parameter, const __FlashStringHelper* value);
	bool setHttpParameter(const __FlashStringHelper* parameter, const char* value);
	bool setHttpParameter(const __FlashStringHelper* parameter, const int8_t value);
	bool setHttpBody(const char* body);
	bool httpInit();
	bool httpEnd();

	bool setBearerSetting(const __FlashStringHelper *parameter, const char* value);

public:
	SIM808(uint8_t resetPin, uint8_t pwrKeyPin, uint8_t statusPin);
	~SIM808();	

	bool powered();
	bool powerOnOff(bool power);
	SIM808ChargingStatus getChargingState();

	bool setPhoneFunctionality(SIM808_PHONE_FUNCTIONALITY fun);
	bool setSlowClock(SIM808_SLOW_CLOCK mode);

	void init();
	void reset();

	size_t sendCommand(const char* cmd, char* response);

	bool setEcho(SIM808_ECHO mode);
	bool simUnlock(const char* pin);
	size_t getSimState(char* state);
	size_t getImei(char* imei);

	SIM808SignalQualityReport getSignalQuality();

	bool setSmsMessageFormat(SIM808_SMS_MESSAGE_FORMAT format);
	bool sendSms(const char* addr, const char* msg);

	bool getGprsPowerState(bool *state);
	bool enableGprs(const char* apn);
	bool enableGprs(const char* apn, const char* user, const char *password);
	bool disableGprs();
	SIM808RegistrationStatus getNetworkRegistrationStatus();

	bool getGpsPowerState(bool *state);
	bool enableGps();
	bool disableGps();
	SIM808_GPS_STATUS getGpsStatus();
	bool getGpsField(const char* response, SIM808_GPS_FIELD field, uint8_t* result);
	bool getGpsField(const char* response, SIM808_GPS_FIELD field, float* result);
	void getGpsField(const char* response, SIM808_GPS_FIELD field, char** result);
	bool getGpsPosition(char* response);


	uint16_t httpGet(const char* url, char* response, size_t responseSize);
	uint16_t httpPost(const char* url, const __FlashStringHelper* contentType, const char* body, char* response, size_t responseSize);	
};


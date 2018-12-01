#pragma once

#include <SIMComAT.h>
#include "SIM808.Types.h"

#define HTTP_TIMEOUT 10000L
#define GPS_ACCURATE_FIX_MIN_SATELLITES 4

class SIM808 : public SIMComAT
{
private:
	uint8_t _resetPin;
	uint8_t _statusPin;
	uint8_t _pwrKeyPin;
	const char* _userAgent;

	/**
	 * Wait for the device to be ready to accept communcation.
	 */
	void waitForReady();	

	/**
	 * Set all the parameters up for a HTTP request to be fired next.
	 */
	bool setupHttpRequest(const char* url);
	/**
	 * Fire a HTTP request and return the server response code and body size.
	 */
	bool fireHttpRequest(const SIM808_HTTP_ACTION action, uint16_t *statusCode, size_t *dataSize);
	/**
	 * Read the last HTTP response body into response.
	 */
	bool readHttpResponse(char *response, size_t responseSize, size_t dataSize);	
	bool setHttpParameter(ATConstStr parameter, ATConstStr value);
	bool setHttpParameter(ATConstStr parameter, const char * value);
	bool setHttpParameter(ATConstStr parameter, uint8_t value);
	/**
	 * Set the HTTP body of the next request to be fired.
	 */
	bool setHttpBody(const char* body);
	/**
	 * Initialize the HTTP service.
	 */
	bool httpInit();
	/**
	 * Terminate the HTTP service.
	 */
	bool httpEnd();

	/**
	 * Set one of the bearer settings for application based on IP.
	 */
	// bool setBearerSetting(ATConstStr parameter, const char* value);

public:
	SIM808(uint8_t resetPin, uint8_t pwrKeyPin, uint8_t statusPin);
	~SIM808();	

	/**
	 * Get a boolean indicating wether or not the device is currently powered on.
	 */
	bool powered();
	/**
	 * Power on or off the device only if the requested state is different than the actual state.
	 */
	bool powerOnOff(bool power);
	/**
	 * Get current charging state, level and voltage from the device.
	 */
	SIM808ChargingStatus getChargingState();

	/**
	 * Get current phone functionality mode.
	 */
	SIM808_PHONE_FUNCTIONALITY getPhoneFunctionality();
	/**
	 * Set the phone functionality mode.
	 */
	bool setPhoneFunctionality(SIM808_PHONE_FUNCTIONALITY fun);
	/**
	 * Configure slow clock, allowing the device to enter sleep mode.
	 */
	bool setSlowClock(SIM808_SLOW_CLOCK mode);

	void init();
	void reset();

	/**
	 * Send an already formatted command and read a single line response. Useful for unimplemented commands.
	 */
	size_t sendCommand(const char* cmd, char* response, size_t responseSize);

	bool setEcho(SIM808_ECHO mode);
	/**
	 * Unlock the SIM card using the provided pin. Beware of failed attempts !
	 */
	bool simUnlock(const char* pin);
	/**
	 * Get a string indicating the current sim state.
	 */
	size_t getSimState(char* state, size_t stateSize);
	/**
	 * Get the device IMEI number.
	 */
	size_t getImei(char* imei, size_t imeiSize);

	/**
	 * Get current GSM signal quality, estimated attenuation in dB and error rate.
	 */
	SIM808SignalQualityReport getSignalQuality();

	bool setSmsMessageFormat(SIM808_SMS_MESSAGE_FORMAT format);
	/**
	 * Send a SMS to the provided number.
	 */
	bool sendSms(const char* addr, const char* msg);

	/**
	 * Get a boolean indicating wether or not GPRS is currently enabled.
	 */
	bool getGprsPowerState(bool *state);
	/**
	 * Reinitiliaze and enable GPRS.
	 */
	bool enableGprs(const char* apn, const char* user = NULL, const char *password = NULL);
	/**
	 * Shutdown GPRS properly.
	 */
	bool disableGprs();
	/**
	 * Get the device current network registration status.
	 */
	SIM808_NETWORK_REGISTRATION_STATE getNetworkRegistrationStatus();

	/**
	 * Get a boolean indicating wether or not GPS is currently powered on.
	 */
	bool getGpsPowerState(bool *state);
	/**
	 * Power on or off the gps only if the requested state is different than the actual state.
	 */
	bool powerOnOffGps(bool power);
	/**
	 * Get the latest GPS parsed sequence and a value indicating the current
	 * fix status. 
	 * Response is only filled if a fix is acquired.
	 * If a fix is acquired, FIX or ACCURATE_FIX will be returned depending on 
	 * wether or not the satellites used is greater than minSatellitesForAccurateFix.
	 */
	SIM808_GPS_STATUS getGpsStatus(char * response, size_t responseSize, uint8_t minSatellitesForAccurateFix = GPS_ACCURATE_FIX_MIN_SATELLITES);
	/**
	 * Extract the specified field from the GPS parsed sequence as a uint16_t.
	 */
	bool getGpsField(const char* response, SIM808_GPS_FIELD field, uint16_t* result);
	/**
	 * Extract the specified field from the GPS parsed sequence as a float.
	 */
	bool getGpsField(const char* response, SIM808_GPS_FIELD field, float* result);
	/**
	 * Return a pointer to the specified field from the GPS parsed sequence.
	 */
	void getGpsField(const char* response, SIM808_GPS_FIELD field, char** result);
	/**
	 * Get and return the latest GPS parsed sequence.
	 */
	bool getGpsPosition(char* response, size_t responseSize);


	/**
	 * Send an HTTP GET request and read the server response within the limit of responseSize.
	 * 
	 * HTTP and HTTPS are supported, based on he provided URL. Note however that HTTPS request
	 * have a high failure rate that make them unusuable reliably.
	 */
	uint16_t httpGet(const char* url, char* response, size_t responseSize);
	/**
	 * Send an HTTP POST request and read the server response within the limit of responseSize.
	 * 
	 * HTTP and HTTPS are supported, based on he provided URL. Note however that HTTPS request
	 * have a high failure rate that make them unusuable reliably.
	 */
	uint16_t httpPost(const char* url, ATConstStr contentType, const char* body, char* response, size_t responseSize);	
};


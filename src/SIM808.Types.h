#pragma once

#include <Arduino.h>

enum class SIM808_ECHO : uint8_t
{
	OFF = 0,
	ON = 1
};

enum class SIM808_SMS_MESSAGE_FORMAT : uint8_t
{
	PDU = 0,
	TEXT = 1
};

/**
 * List of supported HTTP methods.
 */
enum class SIM808_HTTP_ACTION : uint8_t
{
	GET = 0,
	POST = 1,
	HEAD = 2
};

/**
 * Fields returned by the AT+HTTPACTION command.
 */
enum class SIM808_HTTP_ACTION_RESPONSE : uint8_t
{
	METHOD = 0,			///< HTTP method.
	STATUS_CODE = 1,	///< Status code responded by the remote server.
	DATA_LEN = 2		///< Response body length.
};

/**
 * Fields returned by the AT+CGREG command.
 */
enum class SIM808_REGISTRATION_STATUS_RESPONSE : uint8_t
{
	N = 0,		///< Controls network registration unsolicited result code.
	STAT = 1,	///< Current registration status. See SIM808_NETWORK_REGISTRATION_STATE.
	LAC = 2,	///< Location information.
	CI = 3		///< Location information.
};

/**
 * Fields return by the AT+CSQ command.
 */
enum class SIM808_SIGNAL_QUALITY_RESPONSE : uint8_t
{
	SIGNAL_STRENGTH = 0,	///< Received Signal Strength Indication
	BIT_ERROR_RATE = 1		///< Bit Error Rate
};

enum class SIM808_PHONE_FUNCTIONALITY : int8_t
{
	FAIL = -1,		///< Reading the current phone functionality has failed.
	MINIMUM = 0,	///< Minimum functionality.
	FULL = 1,		///< Full functionality (default on device power on).
	RF_DISABLED = 4	///< Disable phone both transmit and receive RF circuit.
};

enum class SIM808_GPS_STATUS : int8_t
{
	FAIL = -1,			///< Reading the current GPS position has failed.
	OFF = 0,			///< GPS is off.
	NO_FIX = 1,			///< A fix is not acquired yet.
	FIX = 2,			///< A fix is acquired.
	ACCURATE_FIX = 3	///< An accurate fix is acquired, using more than GPS_ACCURATE_FIX_MIN_SATELLITES.
};

enum class SIM808_GPS_FIELD : uint8_t
{
	UTC = 2,			///< UTC date time, as yyyyMMddhhmmss.000.
	LATITUDE = 3,		///< Latitude in degrees.
	LONGITUDE = 4,		///< Longitude in degress.
	ALTITUDE = 5,		///< Altitude in meters.
	SPEED = 6,			///< Speed over ground in km/h.
	COURSE = 7,			///< Course over ground in degrees.
	GPS_IN_VIEW = 14,	///< GPS satellites in view.
	GNSS_USED = 15		///< GPS satellites used to acquire the position.
};

enum class SIM808_BATTERY_CHARGE_FIELD : uint8_t
{
	BCS = 0,	///< Battery Charge State.
	BCL = 1,	///< Battery Charge Level.
	VOLTAGE = 2	///< Battery voltage.
};

enum class SIM808_SLOW_CLOCK : uint8_t
{
	DISABLE = 0,	///< Disables slow clock, module will not enter sleep mode
	ENABLE = 1,		///< Enables slow clock, controlled by DTR pin.
	AUTO = 2		///< Enables slow clock automatically.
};

enum class SIM808_CHARGING_STATE : int8_t
{
	ERROR = -1,			///< Reading the current charging status has failed.
	NOT_CHARGING = 0,	///< Not charging.
	CHARGING = 1,		///< Charging.
	CHARGING_DONE = 2	///< Plugged in, but charging done.
};

enum class SIM808_NETWORK_REGISTRATION_STATE : int8_t
{
	ERROR = -1,			///< Reading the current network registration status as failed.
	NOT_SEARCHING = 0,	///< Not searching.
	REGISTERED = 1,		///< Registered to the home network.
	SEARCHING  = 2,		///< Not registered but searching for a network to register.
	DENIED = 3,			///< Registration has been denied by the provider. Stopped searching.
	UNKNOWN = 4,		///< Unknown
	ROAMING = 5			///< Registered to a network that is not the home network.
};

struct SIM808SignalQualityReport
{
	uint8_t rssi;		///< Received Signal Strength Indication, from 0 (worst) to 31 (best). 99 means unknown. 
	uint8_t ber;		///< Bit Error Rate, from 0 to 7. 99 means unknown.
	int8_t attenuation;	///< Estimad signal attenuation from rssi, expressed in dBm.
};

struct SIM808ChargingStatus
{
	SIM808_CHARGING_STATE state;	///< Current charging state.
	int8_t level;					///< Battery level, expressed as a percentage.
	int16_t voltage;				///< Battery level, expressed in mV.
};
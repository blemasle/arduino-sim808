#pragma once

#include <Arduino.h>

enum class SIM808Echo : uint8_t
{
	Off = 0,
	On = 1
};

enum class SIM808SmsMessageFormat : uint8_t
{
	Pdu = 0,
	Text = 1
};

/**
 * List of supported HTTP methods.
 */
enum class SIM808HttpAction : uint8_t
{
	Get = 0,
	Post = 1,
	Head = 2
};

/**
 * Fields returned by the AT+HTTPACTION command.
 */
enum class SIM808HttpActionResponse : uint8_t
{
	Method = 0,		///< HTTP method.
	StatusCode = 1,	///< Status code responded by the remote server.
	DataLen = 2		///< Response body length.
};

/**
 * Fields returned by the AT+CGREG command.
 */
enum class SIM808RegistrationStatusResponse : uint8_t
{
	N = 0,		///< Controls network registration unsolicited result code.
	Stat = 1,	///< Current registration status. See SIM808NetworkRegistrationState.
	Lac = 2,	///< Location information.
	Ci = 3		///< Location information.
};

/**
 * Fields return by the AT+CSQ command.
 */
enum class SIM808SignalQualityResponse : uint8_t
{
	SignalStrength = 0,	///< Received Signal Strength Indication
	BitErrorrate = 1	///< Bit Error Rate
};

enum class SIM808PhoneFunctionality : int8_t
{
	Fail = -1,		///< Reading the current phone functionality has failed.
	Minimum = 0,	///< Minimum functionality.
	Full = 1,		///< Full functionality (default on device power on).
	Disabled = 4	///< Disable phone both transmit and receive RF circuit.
};

enum class SIM808GpsStatus : int8_t
{
	Fail = -1,			///< Reading the current GPS position has failed.
	Off = 0,			///< GPS is off.
	NoFix = 1,			///< A fix is not acquired yet.
	Fix = 2,			///< A fix is acquired.
	AccurateFix = 3		///< An accurate fix is acquired, using more than GPS_ACCURATE_FIX_MIN_SATELLITES.
};

enum class SIM808GpsField : uint8_t
{
	Utc = 2,			///< UTC date time, as yyyyMMddhhmmss.000.
	Latitude = 3,		///< Latitude in degrees.
	Longitude = 4,		///< Longitude in degress.
	Altitude = 5,		///< Altitude in meters.
	Speed = 6,			///< Speed over ground in km/h.
	Course = 7,			///< Course over ground in degrees.
	GpsInView = 14,		///< GPS satellites in view.
	GnssUsed = 15		///< GPS satellites used to acquire the position.
};

enum class SIM808BatteryChargeField : uint8_t
{
	Bcs = 0,	///< Battery Charge State.
	Bcl = 1,	///< Battery Charge Level.
	Voltage = 2	///< Battery voltage.
};

enum class SIM808SlowClock : uint8_t
{
	Disable = 0,	///< Disables slow clock, module will not enter sleep mode
	Enable = 1,		///< Enables slow clock, controlled by DTR pin.
	Auto = 2		///< Enables slow clock automatically.
};

enum class SIM808ChargingState : int8_t
{
	Error = -1,			///< Reading the current charging status has failed.
	NotCharging = 0,	///< Not charging.
	Charging = 1,		///< Charging.
	ChargingDone = 2	///< Plugged in, but charging done.
};

enum class SIM808NetworkRegistrationState : int8_t
{
	Error = -1,			///< Reading the current network registration status as failed.
	NotSearching = 0,	///< Not searching.
	Registered = 1,		///< Registered to the home network.
	Searching  = 2,		///< Not registered but searching for a network to register.
	Denied = 3,			///< Registration has been denied by the provider. Stopped searching.
	Unknown = 4,		///< Unknown
	Roaming = 5			///< Registered to a network that is not the home network.
};

struct SIM808SignalQualityReport
{
	uint8_t rssi;		///< Received Signal Strength Indication, from 0 (worst) to 31 (best). 99 means unknown. 
	uint8_t ber;		///< Bit Error Rate, from 0 to 7. 99 means unknown.
	int8_t attenuation;	///< Estimad signal attenuation from rssi, expressed in dBm.
};

struct SIM808ChargingStatus
{
	SIM808ChargingState state;	///< Current charging state.
	int8_t level;					///< Battery level, expressed as a percentage.
	int16_t voltage;				///< Battery level, expressed in mV.
};
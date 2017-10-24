#pragma once

#include "Arduino.h"

enum class SIM808_ECHO : uint8_t
{
	ON = 0,
	OFF = 1
};

enum class SIM808_SMS_MESSAGE_FORMAT : uint8_t
{
	PDU = 0,
	TEXT = 1
};

enum class SIM808_HTTP_ACTION : uint8_t
{
	GET = 0,
	POST = 1,
	HEAD = 2
};

enum class SIM808_HTTP_ACTION_RESPONSE : uint8_t
{
	METHOD = 0,
	STATUS_CODE = 1,
	DATA_LEN = 2
};

enum class SIM808_REGISTRATION_STATUS_RESPONSE : uint8_t
{
	N = 0,
	STAT = 1,
	LAC = 2,
	CI = 3
};

enum class SIM808_PHONE_FUNCTIONALITY : uint8_t
{
	MINIMUM = 0,
	FULL = 1,
	DISABLED = 4
};

enum class SIM808_GPS_STATUS : int8_t
{
	FAIL = -1,
	OFF = 0,
	NO_FIX = 1,
	FIX = 2,
	ACCURATE_FIX = 3
};

struct SIM808RegistrationStatus
{
	byte n;
	byte stat;
};
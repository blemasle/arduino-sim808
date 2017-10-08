#pragma once

#include "Arduino.h"

enum SIM808_SMS_MESSAGE_FORMAT
{
	PDU = 0,
	TEXT = 1
};

enum SIM808_HTTP_ACTION
{
	GET = 0,
	POST = 1,
	HEAD = 2
};

enum SIM808_HTTP_ACTION_RESPONSE
{
	METHOD = 0,
	STATUS_CODE = 1,
	DATA_LEN = 2
};

enum SIM808_REGISTRATION_STATUS_RESPONSE
{
	N = 0,
	STAT = 1,
	LAC = 2,
	CI = 3
};

enum SIM808_PHONE_FUNCTIONALITY
{
	MINIMUM = 0,
	FULL = 1,
	DISABLED = 4
};

enum SIM808_GPS_STATUS
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
#pragma once

#include <Arduino.h>

#define PSTRPTR(x) reinterpret_cast<const __FlashStringHelper *>(x)
#define SIM808_COMMAND(name, text) const char SIM808_COMMAND_##name[] PROGMEM = text
#define SIM808_COMMAND_PARAMETER(category, name) const char SIM808_COMMAND_PARAMETER_##category##_##name[] PROGMEM = #name

#define SIM808_TOKEN(name) const char SIM808_TOKEN_##name[] PROGMEM = #name
#define SIM808_TOKEN_COMPLEX(name, text) const char SIM808_TOKEN_##name[] PROGMEM = text

SIM808_TOKEN(OK);
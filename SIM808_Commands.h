#pragma once

#include "Arduino.h"

#define PSTRPTR(x) reinterpret_cast<const __FlashStringHelper *>(x)
#define SIM808_COMMAND(name, text) const char SIM808_COMMAND_##name[] PROGMEM = text

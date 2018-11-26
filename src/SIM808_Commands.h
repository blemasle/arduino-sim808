#pragma once

#include <Arduino.h>

#if defined(__AVR__)
    #define S_PROGMEM PROGMEM
    typedef const __FlashStringHelper* Sim808ConstStr;
    #define SFP(x)  (reinterpret_cast<Sim808ConstStr>(x))
    #define SFPT(x)  (reinterpret_cast<const char*>(x))
    #define SF(x)   F(x)
#else
    #define S_PROGMEM
    typedef const char* Sim808ConstStr;
    #define SFP(x)  x
    #define SFPT(x)  x
    #define SF(x)   x
#endif

#define TOKEN_TEXT(name, text) const char TOKEN_##name[] PROGMEM = #text
#define TOKEN(name) TOKEN_TEXT(name, name)

#define AT_COMMAND(name, text) const char AT_COMMAND_##name[] PROGMEM = #text
#define AT_COMMAND_PARAMETER(category, name) const char AT_COMMAND_PARAMETER_##category##_##name[] PROGMEM = #name
#define PSTRPTR(x) reinterpret_cast<const __FlashStringHelper *>(x)

TOKEN(AT);
TOKEN(OK);
TOKEN(ERROR);
TOKEN_TEXT(NL, "\n");

#define PSTRPTR(x) reinterpret_cast<const __FlashStringHelper *>(x)
#define SIM808_COMMAND(name, text) const char SIM808_COMMAND_##name[] PROGMEM = text
#define SIM808_COMMAND_PARAMETER(category, name) const char SIM808_COMMAND_PARAMETER_##category##_##name[] PROGMEM = #name

#define SIM808_TOKEN(name) const char SIM808_TOKEN_##name[] PROGMEM = #name
#define SIM808_TOKEN_COMPLEX(name, text) const char SIM808_TOKEN_##name[] PROGMEM = text

SIM808_TOKEN(OK);
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

#define TOKEN_TEXT(name, text) const char TOKEN_##name[] S_PROGMEM = text
#define TOKEN(name) TOKEN_TEXT(name, #name)

#define AT_COMMAND(name, text) const char AT_COMMAND_##name[] S_PROGMEM = text
#define AT_COMMAND_PARAMETER(category, name) const char AT_COMMAND_PARAMETER_##category##_##name[] S_PROGMEM = #name

TOKEN(AT);
TOKEN(OK);
TOKEN(ERROR);
TOKEN_TEXT(READ, "?");
TOKEN_TEXT(WRITE, "=");
TOKEN_TEXT(QUOTE, "\"");
TOKEN_TEXT(RESPONSE, ":");
TOKEN_TEXT(COMMA, ":");
TOKEN_TEXT(NL, "\n");
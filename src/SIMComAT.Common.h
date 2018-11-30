#pragma once

#include <Arduino.h>

#if defined(__AVR__)
    typedef const __FlashStringHelper* ATConstStr;

    #define S_PROGMEM PROGMEM
    #define S_F(x)   F(x)
    #define TO_F(x) (reinterpret_cast<ATConstStr>(x))
    #define TO_P(x) (reinterpret_cast<const char*>(x))
#else
    typedef const char* ATConstStr;

    #define S_PROGMEM
    #define S_F(x)   x
    #define TO_F(x) x
    #define TO_P(x) x
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
TOKEN_TEXT(NL, "\n");
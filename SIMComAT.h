#pragma once

#include "Arduino.h"
#include "ArduinoLog.h"
#include "SIM808_Commands.h"

#define _SIM808_DEBUG _DEBUG

const char ARROW_LEFT[] PROGMEM = "<--";
const char ARROW_RIGHT[] PROGMEM = "-->";

#ifdef _SIM808_DEBUG
#define SIM808_PRINT(...) _debug.verbose(__VA_ARGS__)
#define SIM808_PRINT_CHAR(x) Serial.print((char)x)
#define SIM808_PRINT_P(fmt, ...) _debug.verbose(F(fmt "\n"), __VA_ARGS__)
#define SIM808_PRINT_SIMPLE_P(fmt) _debug.verbose(F(fmt "\n"))

#define RECEIVEARROW _debug.verbose(PSTRPTR(ARROW_LEFT))
#define SENDARROW _debug.verbose(PSTRPTR(ARROW_RIGHT))
#else
#define SIM808_PRINT(x)
#define SIM808_PRINT_CHAR(x)
#define SIM808_PRINT_P(x, ...)
#define SIM808_PRINT_SIMPLE_P(x)
#define RECEIVEARROW 
#define SENDARROW
#endif // _DEBUG

#define BUFFER_SIZE 128
#define SIMCOMAT_DEFAULT_TIMEOUT 1000

class SIMComAT : public Stream
{
private:

protected:
	Stream* _port;
	Logging _output;
#ifdef _SIM808_DEBUG
	Logging _debug;
#endif

	char replyBuffer[BUFFER_SIZE];
	
	void flushInput();
	void send();

	size_t readLine(uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	size_t sendGetResponse(const __FlashStringHelper* msg, char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT); //TODO : use templates (like ArduinoLog) ?
	size_t sendGetResponse(const char* msg, char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	size_t sendGetResponse(char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);

	bool sendAssertResponse(const __FlashStringHelper *msg, const __FlashStringHelper *expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	bool sendAssertResponse(const __FlashStringHelper *expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);

	bool assertResponse(const __FlashStringHelper *expectedResponse);

	char* find(const char* str, char divider, uint8_t index); //TODO : rename

	bool parse(const char* str, char divider, uint8_t index, uint8_t* result);
	bool parse(const char* str, char divider, uint8_t index, int8_t* result);
	bool parse(const char* str, char divider, uint8_t index, uint16_t* result);
	bool parse(const char* str, char divider, uint8_t index, int16_t* result);
	bool parse(const char* str, char divider, uint8_t index, float* result);

	bool parseReply(char divider, uint8_t index, uint8_t* result) { return parse(replyBuffer, divider, index, result); }
	bool parseReply(char divider, uint8_t index, int8_t* result) { return parse(replyBuffer, divider, index, result); }
	bool parseReply(char divider, uint8_t index, uint16_t* result) { return parse(replyBuffer, divider, index, result); }
	bool parseReply(char divider, uint8_t index, int16_t* result) { return parse(replyBuffer, divider, index, result); }
	bool parseReply(char divider, uint8_t index, float* result) { return parse(replyBuffer, divider, index, result); }

	virtual void init()=0;
public:	
	void begin(Stream& port);

#pragma region Stream implementation
	int available() { return _port->available(); }
	size_t write(uint8_t x) { SIM808_PRINT_CHAR(x); return _port->write(x); }
	int read() { return _port->read(); }
	int peek() { return _port->peek(); }
	void flush() { return _port->flush(); }
#pragma endregion

};
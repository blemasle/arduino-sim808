#pragma once

#include "Arduino.h"

//#define _SIM808_DEBUG _DEBUG

#ifdef _SIM808_DEBUG
#define SIM808_PRINT(...) Serial.print(__VA_ARGS__)
#define SIM808_PRINTLN(...) Serial.println(__VA_ARGS__);
#define RECEIVEARROW SIM808_PRINT(F("<--"))
#define SENDARROW SIM808_PRINT(F("-->"))
#else
#define SIM808_PRINT(x)
#define SIM808_PRINTLN(x)
#define RECEIVEARROW 
#define SENDARROW
#endif // _DEBUG

#define BUFFER_SIZE 256
#define SIMCOMAT_DEFAULT_TIMEOUT 1000

class SIMComAT : public Stream
{
private:
	Stream* _port;

	char* find(const char* str, char divider, uint8_t index); //TODO : rename
protected:
	char replyBuffer[BUFFER_SIZE];
	
	void flushInput();
	void send();

	size_t readLine(uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	size_t sendGetResponse(const char* msg, char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	size_t sendGetResponse(char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);

	bool sendAssertResponse(const char* msg, const char* expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	bool sendAssertResponse(const char* expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);

	bool assertResponse(const char* expectedResponse);

	bool parse(const char* str, char divider, uint8_t index, uint8_t* result);
	bool parse(const char* str, char divider, uint8_t index, int8_t* result);
	bool parse(const char* str, char divider, uint8_t index, uint16_t* result);
	bool parse(const char* str, char divider, uint8_t index, int16_t* result);
	bool parse(const char* str, char divider, uint8_t index, float* result);

	bool parseReply(char divider, uint8_t index, uint8_t* result);
	bool parseReply(char divider, uint8_t index, int8_t* result);
	bool parseReply(char divider, uint8_t index, uint16_t* result);
	bool parseReply(char divider, uint8_t index, int16_t* result);
	bool parseReply(char divider, uint8_t index, float* result);

	virtual void init();
public:	
	void begin(Stream& port);

#pragma region Stream implementation
	int available() { return _port->available(); }
	size_t write(uint8_t x) { SIM808_PRINT((char)x); return _port->write(x); }
	int read() { return _port->read(); }
	int peek() { return _port->peek(); }
	void flush() { return _port->flush(); }
#pragma endregion

};
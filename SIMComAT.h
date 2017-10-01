#pragma once

#include "Arduino.h"
//#include <inttypes.h>
//#include <Stream.h>
//#include <avr/pgmspace.h>

#ifdef _DEBUG
#define PRINT(...) Serial.print(__VA_ARGS__)
#define PRINTLN(...) Serial.println(__VA_ARGS__);
#define RECEIVEARROW PRINT(F("<--"))
#define SENDARROW PRINT(F("-->"))
#else
#define PRINT(x)
#define PRINTLN(x)
#define RECEIVEARROW 
#define SENDARROW
#endif // _DEBUG

#define BUFFER_SIZE 256
#define SIMCOMAT_DEFAULT_TIMEOUT 1000

class SIMComAT : public Stream
{
private:
	Stream* _port;

protected:
	char _replyBuffer[BUFFER_SIZE];
	
	void flushInput();
	void send();

	size_t readLine(uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	size_t sendGetResponse(const char* msg, char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	size_t sendGetResponse(char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);

	bool sendAssertResponse(const char* msg, const char* expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	bool sendAssertResponse(const char* expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);

	bool assertResponse(const char* expectedResponse);

	bool parseReply(char divider, uint8_t index, uint8_t* result);
	bool parseReply(char divider, uint8_t index, uint16_t* result);

	virtual void init();
public:	
	void begin(Stream& port);

#pragma region Stream implementation
	int available() { return _port->available(); }
	size_t write(uint8_t x) { PRINT((char)x); return _port->write(x); }
	int read() { return _port->read(); }
	int peek() { return _port->peek(); }
	void flush() { return _port->flush(); }
#pragma endregion

};
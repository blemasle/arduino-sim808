#pragma once

#include <Arduino.h>
#include <ArduinoLog.h>
#include "SIM808_Commands.h"

#define _SIM808_DEBUG _DEBUG

#if _SIM808_DEBUG

const char ARROW_LEFT[] PROGMEM = "<--";
const char ARROW_RIGHT[] PROGMEM = "-->";

#define SIM808_PRINT(...) _debug.verbose(__VA_ARGS__)
#define SIM808_PRINT_CHAR(x) Serial.print((char)x)
#define SIM808_PRINT_P(fmt, ...) _debug.verbose(F(fmt "\n"), __VA_ARGS__)
#define SIM808_PRINT_SIMPLE_P(fmt) _debug.verbose(F(fmt "\n"))

#define RECEIVEARROW _debug.verbose(PSTRPTR(ARROW_LEFT))
#define SENDARROW _debug.verbose(PSTRPTR(ARROW_RIGHT))
#else
#define SIM808_PRINT(...)
#define SIM808_PRINT_CHAR(x)
#define SIM808_PRINT_P(x, ...)
#define SIM808_PRINT_SIMPLE_P(x)
#define RECEIVEARROW 
#define SENDARROW
#endif // _DEBUG

//TODO : the buffer size could be greatly reduced (around 64 bytes) if SIM808.Gps.cpp is reworked
#define BUFFER_SIZE 128
#define SIMCOMAT_DEFAULT_TIMEOUT 1000

typedef 
class SIMComAT : public Stream
{
private:

protected:
	Stream* _port;
	Logging _output;
#if _SIM808_DEBUG
	Logging _debug;
#endif

	char replyBuffer[BUFFER_SIZE];
	
	template<typename T> void writeStream(T last)
	{
		print(last);
	}

	template<typename T, typename... Args> void writeStream(T head, Args... tail)
	{
		print(head);
		writeStream(tail...);
	}

	template<typename... Args> void sendAT(Args... cmd)
	{
		writeStream(TOKEN_AT, cmd..., TOKEN_NL);
	}

	template<typename T, typename... Args> void sendFormatAT(T format, Args... args)
	{
		writeStream(TOKEN_AT);
		_output.verbose(format, args...);
		writeStream(TOKEN_NL);
	}

	void readNextLine(uint32_t timeout);
	int8_t waitResponse(
		Sim808ConstStr s1 = SFP(TOKEN_OK),
		Sim808ConstStr s2 = SFP(TOKEN_ERROR),
		Sim808ConstStr s3 = NULL,
		Sim808ConstStr s4 = NULL) {
			return waitResponse(SIMCOMAT_DEFAULT_TIMEOUT, s1, s2, s3, s4);
		};

	int8_t waitResponse(uint32_t timeout, 
		Sim808ConstStr s1 = SFP(TOKEN_OK),
		Sim808ConstStr s2 = SFP(TOKEN_ERROR),
		Sim808ConstStr s3 = NULL,
		Sim808ConstStr s4 = NULL);
		
	/**
	 * Flush all lines currently in the buffer.
	 */
	void flushInput();
	/**
	 * Send a line return, effectively validating the current command. 
	 */
	void send();

	/**
	 * Read the next line into replyBuffer or bail out after timeout.
	 */
	size_t readLine(uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);

	/**
	 * Send a command and wait for a response back.
	 */
	template<typename T>size_t sendGetResponse(T msg, char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	/**
	 * Validate the current command and wait for a response back.
	 */
	size_t sendGetResponse(char* response, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	/**
	 * Copy the content of response into replyBuffer.
	 */
	size_t copyResponse(char *response);
	size_t safeCopy(const char *src, char *dst);
	
	/**
	 * Send a command and check that the response matches the expectedResponse.
	 */
	bool sendAssertResponse(const __FlashStringHelper *msg, const __FlashStringHelper *expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);
	/**
	 * Validate the current command and check that the response matches the expectedResponse.
	 */
	bool sendAssertResponse(const __FlashStringHelper *expectedResponse, uint16_t timeout = SIMCOMAT_DEFAULT_TIMEOUT);

	/**
	 * Check that replyBuffer matches expectedResponse.
	 */
	bool assertResponse(const __FlashStringHelper *expectedResponse);

	/**
	 * Find and return a pointer to the nth field of a string.
	 */
	char* find(const char* str, char divider, uint8_t index); //TODO : rename

	/**
	 * Parse the nth field of a string as a uint8_t.
	 */
	bool parse(const char* str, char divider, uint8_t index, uint8_t* result);
	/**
	 * Parse the nth field of a string as a int8_t.
	 */
	bool parse(const char* str, char divider, uint8_t index, int8_t* result);
	/**
	 * Parse the nth field of a string as a uint16_t.
	 */
	bool parse(const char* str, char divider, uint8_t index, uint16_t* result);
	/**
	 * Parse the nth field of a string as a int16_t.
	 */
	bool parse(const char* str, char divider, uint8_t index, int16_t* result);
	/**
	 * Parse the nth field of a string as a float.
	 */
	bool parse(const char* str, char divider, uint8_t index, float* result);

	/**
	 * Parse the nth field of the reply buffer as a uint8_t.
	 */
	bool parseReply(char divider, uint8_t index, uint8_t* result) { return parse(replyBuffer, divider, index, result); }
	/**
	 * Parse the nth field of the reply buffer as a int8_t.
	 */
	bool parseReply(char divider, uint8_t index, int8_t* result) { return parse(replyBuffer, divider, index, result); }
	/**
	 * Parse the nth field of the reply buffer as a uint16_t.
	 */
	bool parseReply(char divider, uint8_t index, uint16_t* result) { return parse(replyBuffer, divider, index, result); }
	/**
	 * Parse the nth field of the reply buffer as a int16_t.
	 */
	bool parseReply(char divider, uint8_t index, int16_t* result) { return parse(replyBuffer, divider, index, result); }
	/**
	 * Parse the nth field of the reply buffer as a float.
	 */
	bool parseReply(char divider, uint8_t index, float* result) { return parse(replyBuffer, divider, index, result); }

	/**
	 * Perform the minmum commands needed to get the device communication up and running.
	 */
	virtual void init()=0;
public:	
	/**
	 * Begin communicating with the device.
	 */
	void begin(Stream& port);

#pragma region Stream implementation

	int available() { return _port->available(); }
	size_t write(uint8_t x) { SIM808_PRINT_CHAR(x); return _port->write(x); }
	int read() { return _port->read(); }
	int peek() { return _port->peek(); }
	void flush() { return _port->flush(); }
	
#pragma endregion

};
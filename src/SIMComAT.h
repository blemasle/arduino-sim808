#pragma once

#include <Arduino.h>
#include <ArduinoLog.h>
#include "SIMComAT.Common.h"

#define _SIM808_DEBUG _DEBUG

#if _SIM808_DEBUG
	#define SIM808_PRINT(...) _debug.verbose(__VA_ARGS__)
	#define SIM808_PRINT_CHAR(x) Serial.print((char)x)
	#define SIM808_PRINT_P(fmt, ...) _debug.verbose(S_F(fmt "\n"), __VA_ARGS__)
	#define SIM808_PRINT_SIMPLE_P(fmt) _debug.verbose(S_F(fmt "\n"))

	#define RECEIVEARROW _debug.verbose(S_F("<--"))
	#define SENDARROW _debug.verbose(S_F("\n-->"))
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
		SENDARROW;
		writeStream(TO_F(TOKEN_AT), cmd..., TO_F(TOKEN_NL));
	}

	template<typename T, typename... Args> void sendFormatAT(T format, Args... args)
	{
		SENDARROW;
		writeStream(TO_F(TOKEN_AT));
		_output.verbose(format, args...);
		writeStream(TO_F(TOKEN_NL));
	}

	/**
	 * Read and discard all content already waiting to be parsed. 
	 */
	void flushInput();
	/**
	 * Read data into replyBuffer and stops when the buffer is full, a new line is encountered,
	 * or when the timeout is expired.
	 */
	size_t readNext(uint16_t *timeout);
	int8_t waitResponse(
		ATConstStr s1 = TO_F(TOKEN_OK),
		ATConstStr s2 = TO_F(TOKEN_ERROR),
		ATConstStr s3 = NULL,
		ATConstStr s4 = NULL) {
			return waitResponse(SIMCOMAT_DEFAULT_TIMEOUT, s1, s2, s3, s4);
		};

	int8_t waitResponse(uint16_t timeout, 
		ATConstStr s1 = TO_F(TOKEN_OK),
		ATConstStr s2 = TO_F(TOKEN_ERROR),
		ATConstStr s3 = NULL,
		ATConstStr s4 = NULL);
		
	/**
	 * Read the current response line and copy it in response. Start at replyBuffer + shift
	 */
	size_t copyCurrentLine(char *dst, uint16_t shift = 0);
	size_t safeCopy(const char *src, char *dst);
	
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
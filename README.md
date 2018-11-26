# SIM808
This library allows to access some of the features of the [SIM808](https://simcom.ee/documents/?dir=SIM808) GPS & GSM module. It requires at least 3 pins (power, status and reset pins) to work and a TTL Serial.

The library tries to reduces memory consumption as much as possible, but nonetheless use a 128 bytes buffer to communicate with the SIM808 module. When available, SIM808 responses are parsed to ensure that commands are correctly executed by the module. Commands timeouts are also set according to SIMCOM documentation.  

> No default instance is created when the library is included

[Arduino-Log](https://github.com/thijse/Arduino-Log) is used to output formatted commands in a `printf` style. This make implementation of new commands
really easy, and avoid successive prints or string concatenation.

## Features
 * Fine control over the module power management
 * Sending SMS
 * Sending GET and POST [HTTP(s)](#a-note-about-https) requests
 * Acquiring GPS positions, with access to individual fields
 * Reading of the device states (battery, gps, network)

## Why another library ?
Their is a number of libraries out there which support this modem ([Adafruit's FONA](https://github.com/adafruit/Adafruit_FONA), [TinyGSM](https://github.com/vshymanskyy/TinyGSM) for instance), so why build another one ? None fit the needs I had for a project. FONA is more a giant example for testing commands individually and I was getting unreliable results with it. TinyGSM seems great but what it gains in chips support it lacks in fine grained control 
over each modules, which I needed.

This library is then greatly inspired by FONA, which served as the reference implementation, but mostly only support the features I needed for my project and has been
tested thoroughly and successfully in that configuration. It also tries to reduce the final HEX size as this was a real problem for the project it was built (currently using 30660 bytes out of 30720).  

It does *not* have the pretention to become the new SIM808 standard library, but can be useful to others as a source of inspiration or documentation to understand how AT commands works.

## Debugging
 If you need to debug the communication with the SIM808 module, you can either define `_DEBUG` to `1`, or directly change `_SIM808_DEBUG` to `1` in [SIMComAT.h](/src/SIMComAT.h).
 > Be aware that it will greatly increase the final hex size as debug strings are stored in flash.

 ## Usage
 No default instance is created when the library is included. It's up to you to create one with the appropriate parameters.

 ```cpp
#include <SIM808.h>
#include <SoftwareSerial.h>

#define SIM_RST		5	///< SIM808 RESET
#define SIM_RX		6	///< SIM808 RXD
#define SIM_TX		7	///< SIM808 TXD
#define SIM_PWR		9	///< SIM808 PWRKEY
#define SIM_STATUS	8	///< SIM808 STATUS

#define SIM808_BAUDRATE 4800    ///< Control the baudrate use to communicate with the SIM808 module

SoftwareSerial simSerial = SoftwareSerial(SIM_TX, SIM_RX)
SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);

void setup() {
    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);

    sim808.powerOnOff(true);    //power on the SIM808
    sim808.init();
}

void loop() {
    //whatever you need to do
}
 ```
See examples for further usage.

## A note about https

While technically, SIM808 module support HTTPS requests, it is particularly unreliable and sketchy. 7 times out of 10, the request won't succeed. This is a bummer
and I strongly recommand to stick with HTTP requests if you need reliability.
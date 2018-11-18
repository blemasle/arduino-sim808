# SIM808
This library allows to access many of the features of the [SIM808](https://simcom.ee/documents/?dir=SIM808) GPS & GSM module. It requires at least 3 pins to work and a TTL Serial. If using a SoftwareSerial instance, 2 more pins are then required.  

The library tries to reduces memory consumption as much as possible, but nonetheless use a 128 bytes buffer to communicate with the SIM808 module. When available, SIM808 responses are parsed to ensure that commands are correctly executed by the module. Commands timeouts are also set according to SIMCOM documentation.  

> No default instance is created when the library is included

The library use [Arduino-Log](https://github.com/thijse/Arduino-Log/) to print formatted commands to the TTL. While it might not be the best choice regarding final hex size, this library was already used in the project for which this library was initially built.

## Features
 * Fine control over the module power management
 * Sending SMS
 * Sending GET and POST HTTP(s) requests
 * Acquiring GPS positions, with access to individual fields
 * Reading of the device states (battery, charging, gps, network)

 ## Debugging
 If you need to debug the communication with the SIM808 module, you can either define `_DEBUG` to `1`, or directly change `_SIM808_DEBUG` to `1` in [SIMComAT](/src/SIMComAT).
 > Be aware that it will increase the final hex size.

 ## Usage
 Unlike most Arduino library, no default instance is created when the library is included. It's up to you to create one with the appropriate parameters.

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

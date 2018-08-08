# SIM808
This library allows to access many of the features of the [SIM808](https://simcom.ee/documents/?dir=SIM808) GPS & GSM module. It requires at least 3 pins to work and a TTL Serial. If using a SoftwareSerial instance, 2 more pins are then required.  

The library tries to reduces memory consumption as much as possible, but nonetheless use a 128 bytes buffer to communicate with the SIM808 module. When available, SIM808 responses are parsed to ensure that commands are correctly executed by the module. Commands timeouts are also set according to SIMCOM documentation.  

> No default instance is created when the library is included

## Features
 * Fine control over the module power management
 * Sending SMS
 * Sending GET and POST HTTP(s) requests
 * Acquiring GPS positions, with access to individual fields
 * Reading of the device states (battery, charging, gps, network)

 ## Debugging
 If you need to debug the communication with the SIM808 module, you can either define `_DEBUG` to `1`, or directly change `_SIM808_DEBUG` to `1` in [SIMComAT](/src/SIMComAT).
 > Be aware that it will increase the final hex size.

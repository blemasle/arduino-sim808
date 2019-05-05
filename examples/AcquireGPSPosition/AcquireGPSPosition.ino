/**
 * Acquire a GPS position and display it on Serial
 */

#include <SIM808.h>
#include <ArduinoLog.h>

#if defined(__AVR__)
    #include <SoftwareSerial.h>
    #define SIM_SERIAL_TYPE	SoftwareSerial					///< Type of variable that holds the Serial communication with SIM808
    #define SIM_SERIAL		SIM_SERIAL_TYPE(SIM_TX, SIM_RX)	///< Definition of the instance that holds the Serial communication with SIM808    
#else
    #include <HardwareSerial.h>
    #define SIM_SERIAL_TYPE	HardwareSerial					///< Type of variable that holds the Serial communication with SIM808
    #define SIM_SERIAL		SIM_SERIAL_TYPE(2)	            ///< Definition of the instance that holds the Serial communication with SIM808    
#endif

#define SIM_RST		5	///< SIM808 RESET
#define SIM_RX		6	///< SIM808 RXD
#define SIM_TX		7	///< SIM808 TXD
#define SIM_PWR		9	///< SIM808 PWRKEY
#define SIM_STATUS	8	///< SIM808 STATUS

#define SIM808_BAUDRATE 4800    ///< Control the baudrate use to communicate with the SIM808 module
#define SERIAL_BAUDRATE 38400   ///< Controls the serial baudrate between the arduino and the computer
#define NO_FIX_GPS_DELAY 3000   ///< Delay between each GPS read when no fix is acquired
#define FIX_GPS_DELAY  10000    ///< Delay between each GPS read when a fix is acquired

#define POSITION_SIZE   128     ///< Size of the position buffer
#define NL  "\n"

#if defined(__AVR__)
    typedef __FlashStringHelper* __StrPtr;
#else
    typedef const char* __StrPtr;
#endif

SIM_SERIAL_TYPE simSerial = SIM_SERIAL;
SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);
char position[POSITION_SIZE];

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);

    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);

    Log.notice(S_F("Powering on SIM808..." NL));
    sim808.powerOnOff(true);
    sim808.init();

    Log.notice(S_F("Powering on SIM808's GPS..." NL));
	sim808.powerOnOffGps(true);
}

void loop() {
    SIM808GpsStatus status = sim808.getGpsStatus(position, POSITION_SIZE);
    
    if(status < SIM808GpsStatus::Fix) {
        Log.notice(S_F("No fix yet..." NL));
        delay(NO_FIX_GPS_DELAY);
        return;
    }

    uint16_t sattelites;
    float lat, lon;
    __StrPtr state;

    if(status == SIM808GpsStatus::Fix) state = S_F("Normal");
    else state = S_F("Accurate");

    sim808.getGpsField(position, SIM808GpsField::GnssUsed, &sattelites);
    sim808.getGpsField(position, SIM808GpsField::Latitude, &lat);
    sim808.getGpsField(position, SIM808GpsField::Longitude, &lon);

    Log.notice(S_F("%s" NL), position);
    Log.notice(S_F("Fix type: %S" NL), state);
    Log.notice(S_F("Sattelites used : %d" NL), sattelites);
    Log.notice(S_F("Latitude : %F" NL), lat);
    Log.notice(S_F("Longitude : %F" NL), lon);
    
    delay(FIX_GPS_DELAY);
}
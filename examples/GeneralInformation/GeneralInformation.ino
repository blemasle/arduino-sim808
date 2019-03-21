/**
 * Display general information about the SIM808 chip on Serial
 */

#include <SIM808.h>
#include <ArduinoLog.h>
#include <SoftwareSerial.h>

#define SIM_RST		5	///< SIM808 RESET
#define SIM_RX		6	///< SIM808 RXD
#define SIM_TX		7	///< SIM808 TXD
#define SIM_PWR		9	///< SIM808 PWRKEY
#define SIM_STATUS	8	///< SIM808 STATUS

#define SIM808_BAUDRATE 4800    ///< Control the baudrate use to communicate with the SIM808 module
#define SERIAL_BAUDRATE 38400   ///< Controls the serial baudrate between the arduino and the computer
#define NETWORK_DELAY  10000    ///< Delay between each GPS read

#define BUFFER_SIZE 512         ///< Size of the buffer
#define NL  "\n"

SoftwareSerial simSerial = SoftwareSerial(SIM_TX, SIM_RX);
SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);
bool done = false;
char buffer[BUFFER_SIZE];

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);

    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);

    Log.notice(S_F("Powering on SIM808..." NL));
    sim808.powerOnOff(true);
    sim808.init();

    sim808.getImei(buffer, BUFFER_SIZE);
    Log.notice(S_F("IMEI : \"%s\"" NL), buffer);

    sim808.getSimState(buffer, BUFFER_SIZE);
    Log.notice(S_F("SIM card state : \"%s\"" NL), buffer);

    SIM808ChargingStatus charging = sim808.getChargingState();
    switch(charging.state) {
        case SIM808_CHARGING_STATE::CHARGING:
        strcpy_P(buffer, PSTR("CHARGING"));
        break;
        case SIM808_CHARGING_STATE::CHARGING_DONE:
        strcpy_P(buffer, PSTR("CHARGING_DONE"));
        break;
        case SIM808_CHARGING_STATE::ERROR:
        strcpy_P(buffer, PSTR("ERROR"));
        break;
        case SIM808_CHARGING_STATE::NOT_CHARGING:
        strcpy_P(buffer, PSTR("NOT_CHARGING"));
        break;
    }
    Log.notice(S_F("Charging state : %s, %d%% @ %dmV" NL), buffer, charging.level, charging.voltage);

    //you can also send unimplemented simple commands
    sim808.sendCommand("I", buffer, BUFFER_SIZE);
    Log.notice(S_F("ATI response : \"%s\"" NL), buffer);
}

void loop() { }
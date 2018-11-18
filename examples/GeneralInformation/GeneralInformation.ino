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
#define NETWORK_DELAY  10000    ///< Delay between each GPS read

#define BUFFER_SIZE 512         ///< Side of the buffer

SoftwareSerial simSerial = SoftwareSerial(SIM_TX, SIM_RX);
SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);
bool done = false;
char buffer[BUFFER_SIZE];

void setup() {
    Serial.begin(115200);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);

    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);

    Log.notice(F("Powering on SIM808..."));
    sim808.powerOnOff(true);
    sim808.init();

    sim808.getImei(buffer);
    Log.notice(F("IMEI : %s"), buffer);

    sim808.getSimState(buffer);
    Log.notice(F("SIM card state : %s"), buffer);

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
    Log.notice(F("Charging state : %s, %d%% @ %dmV"), buffer, charging.level, charging.voltage);

    //you can also send unimplemented simple commands
    sim808.sendCommand("ATI", buffer);
    Log.notice(F("ATI : %s"), buffer);
}

void loop() { }
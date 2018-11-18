/**
 * Send an HTTP POST request and display the response on Serial
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

#define GPRS_APN    "Vodafone"  ///< Your provider Access Point Name
#define GPRS_USER   NULL        ///< Your provider APN user (usually not needed)
#define GPRS_PASS   NULL        ///< Your provider APN password (usually not needed)

#define BUFFER_SIZE 512         ///< Side of the response buffer

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

    Log.notice(F("Powering on SIM808's GPRS..."));
	sim808.enableGprs(GPRS_APN, GPRS_USER, GPRS_PASS);
}

void loop() {
    if(done) {
        delay(NETWORK_DELAY);
        return;
    }

    SIM808RegistrationStatus status = sim808.getNetworkRegistrationStatus();
    SIM808SignalQualityReport report = sim808.getSignalQuality();

    bool isAvailable = static_cast<int8_t>(status.stat) &
        (static_cast<int8_t>(SIM808_NETWORK_REGISTRATION_STATE::REGISTERED) | static_cast<int8_t>(SIM808_NETWORK_REGISTRATION_STATE::ROAMING))
        != 0;

    if(!isAvailable) {
        Log.notice(F("No network yet..."));
        delay(NETWORK_DELAY);
        return;
    }

    Log.notice(F("Network is ready."));
    Log.notice(F("Attenuation : %d dB, Estimated quality : %d"), report.attenuation, report.ssri);

    strncpy_P(buffer, PSTR("This is the body"), BUFFER_SIZE);
    //notice that we're using the same buffer for both body and response
    uint16_t responseCode = sim808.httpPost("http://httpbin.org/anything", F("text/plain"),  buffer, buffer, BUFFER_SIZE);

    Log.notice(F("Server responsed : %d"), responseCode);
    Log.notice(buffer);

    done = true;
}
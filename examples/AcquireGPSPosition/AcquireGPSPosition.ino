#include <SIM808.h>
#include <SoftwareSerial.h>

#define SIM_RST		5	///< SIM808 RESET
#define SIM_RX		6	///< SIM808 RXD
#define SIM_TX		7	///< SIM808 TXD
#define SIM_PWR		9	///< SIM808 PWRKEY
#define SIM_STATUS	8	///< SIM808 STATUS

#define SIM808_BAUDRATE 4800    ///< Control the baudrate use to communicate with the SIM808 module
#define READ_GPS_DELAY  10000   ///< Delay between each GPS read

SoftwareSerial simSerial = SoftwareSerial(SIM_TX, SIM_RX)
SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);
char position[128];

void setup() {
    Serial.begin(115200);

    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);

    Serial.println("Powering on SIM808...");
    sim808.powerOnOff(true);
    sim808.init();

    Serial.println("Powering on SIM808's GPS...");
	sim808.enableGps();
}

void loop() {
    SIM808_GPS_STATUS status = sim808.getGpsStatus(position);
    
    if(status < SIM808_GPS_STATUS::FIX) {
        Serial.println("No fix yet...");
        delay(READ_GPS_DELAY);
        return;
    }

    Serial.println(position);

    Serial.print("Fix Type:");
    if(status == SIM808_GPS_STATUS::FIX) Serial.print("Normal");
    else Serial.print("Accurate");

    uint8_t sattelites;
    float lat, lon;

    sim808.getGpsField(position, SIM808_GPS_FIELD::GNSS_USED, &sattelites);
    sim808.getGpsField(position, SIM808_GPS_FIELD::LATITUDE, &lat);
    sim808.getGpsField(position, SIM808_GPS_FIELD::LONGITUDE, &lon);

    Serial.print("Sattelites used :");
    Serial.println(tmpByte);

    Serial.print("lat :");
    Serial.print(lat);

    Serial.print("lon :");
    Serial.println(lon);
}
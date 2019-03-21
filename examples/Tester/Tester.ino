/**
 * Provides a serial interface to test functions of the SIM808 library
 */

#include <SoftwareSerial.h>
#include <SIM808.h>
#include <ArduinoLog.h>

#define SIM_RST		5	///< SIM808 RESET
#define SIM_RX		6	///< SIM808 RXD
#define SIM_TX		7	///< SIM808 TXD
#define SIM_PWR		9	///< SIM808 PWRKEY
#define SIM_STATUS	8	///< SIM808 STATUS

#define SIM808_BAUDRATE 4800    ///< Controls the serial baudrate between the arduino and the SIM808 module
#define SERIAL_BAUDRATE 38400   ///< Controls the serial baudrate between the arduino and the computer

#define BUFFER_SIZE 512 ///< Size of the buffer on the main program side
#define NL "\n"

#if defined(__AVR__)
    #define STRING_IS(s1, s2) strcasecmp_P(buffer, PSTR(s2)) == 0
    #define BUFFER_IS(s) STRING_IS(buffer, s)
    #define BUFFER_IS_P(s) strcasecmp_P(buffer, s) == 0

    #define PRINT(s) Serial.print(S_F(s))
    #define PRINT_LN(s) Serial.println(S_F(s))
#else
    #define BUFFER_IS(s) strcasecmp(buffer, s) == 0
    #define BUFFER_IS(s) STRING_IS(buffer, s)
    #define BUFFER_IS_P(s) strcasecmp(buffer, s) == 0

    #define PRINT(s) Serial.print(s)
    #define PRINT_LN(s) Serial.println(s)
#endif


const char UNRECOGNIZED[] S_PROGMEM = "Unrecognized : %s" NL;
const char UNKNOWN[] S_PROGMEM = "Unknown value";

const char YES[] S_PROGMEM = "Yes";
const char NO[] S_PROGMEM = "No";

const char SUCCESS[] S_PROGMEM = "Success";
const char FAILED[] S_PROGMEM = "Failed";
const char ERROR[] S_PROGMEM = "Error";

const char ON[] S_PROGMEM = "On";
const char OFF[] S_PROGMEM = "Off";
const char STATUS[] S_PROGMEM = "Status";
const char ALL[] S_PROGMEM = "All";

const char MAIN[] S_PROGMEM = "Main";
const char GPS[] S_PROGMEM = "Gps";
const char NETWORK[] S_PROGMEM = "Network";

const char MINIMUM[] S_PROGMEM = "Minimum";
const char FULL[] S_PROGMEM = "Full";
const char RF_DISABLED[] S_PROGMEM = "Disabled";

const char SEND[] S_PROGMEM = "Send";
const char DEFAULT_URL[] S_PROGMEM = "http://httpbin.org/anything";

SoftwareSerial simSerial = SoftwareSerial(SIM_TX, SIM_RX);
SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);
char buffer[BUFFER_SIZE];

void usage() {
    PRINT_LN("============= SIM808 Tester =============");
    PRINT_LN("?\tHelp");
    
    PRINT_LN("");
    
    PRINT_LN("reset\t\t\t\tReset the device");
    PRINT_LN("init\t\t\t\tInit the device");
    PRINT_LN("power [on|off] [main|gps|network]\tPower On/Off the specified module");
    PRINT_LN("power status [main|gps|network]\t\tGet the specified module power state");

    PRINT_LN("");

    PRINT_LN("charge\tGet the current charge state");

    PRINT_LN("");

    PRINT_LN("sim status\t\tGet the current SIM state");
    PRINT_LN("sim unlock [pin]\tUnlock the SIM using the provided pin");
    PRINT_LN("imei\t\t\tGet the device IMEI");

    PRINT_LN("");

    PRINT_LN("network functionality\t\t\t\tGet the current network functionality");
    PRINT_LN("network functionality [minimum|full|disabled]\tSet the current network functionality");
    PRINT_LN("network status\t\t\t\t\tGet the current network status");
    PRINT_LN("network quality\t\t\t\t\tGet the current network quality report");

    PRINT_LN("");
    
    PRINT_LN("gps raw\t\t\tGet the GPS raw data");
    PRINT_LN("gps parse [field]\tParse GPS fields, field :");
    PRINT_LN("\tall\t- All fields");
    PRINT_LN("\ttime\t- UTC time");
    PRINT_LN("\tlat\t- Latitude");
    PRINT_LN("\tlon\t- Longitude");
    PRINT_LN("\talt\t- Altitude");
    PRINT_LN("\tspeed\t- Speed over ground");
    PRINT_LN("\tcourse\t- Course over ground");
    PRINT_LN("\tsatview\t- Satellites in view");
    PRINT_LN("\tsatused\t- Satellites used");

    PRINT_LN("");

    PRINT_LN("send at [command]\t\t\t\t\t\tSend an AT command");
    PRINT_LN("send sms [number]\t\t\t\t\t\tSend an SMS");
    PRINT_LN("send http [get|post] [url?=http://httpbin.org/anything]\t\tSend an HTTP request");

    PRINT_LN("");
    PRINT_LN("");
}

/**
 * Read the next line or word into the buffer.
 * Returns true if this was the last word of the line.
 */
bool readNext(bool line = false) {
    char c;
    char *p = buffer;
    memset(p, 0, BUFFER_SIZE);

    do {
        while(!Serial.available()) delay(100);

        c = Serial.read();
        if((!line && isspace(c)) || c == '\n') break;

        *p = c;
        p++;
    } while(true);

    *p = '\0';
    return c == '\n';
}

void unrecognized() {
    Log.error(TO_F(UNRECOGNIZED), buffer);
    while(Serial.available()) Serial.read();
}

void reset() {
    Log.notice(S_F("Resetting SIM808..." NL));
    sim808.reset();
}

void init_() {
    Log.notice(S_F("Init SIM808..." NL));
    sim808.init();
}

void power() {
    readNext();

    if(BUFFER_IS_P(STATUS)) {
        bool status;
        __FlashStringHelper * statusText;
        readNext();
        
        if(BUFFER_IS_P(MAIN)) status = sim808.powered();
        else if (BUFFER_IS_P(GPS)) sim808.getGpsPowerState(&status);
        else if (BUFFER_IS_P(NETWORK)) sim808.getGprsPowerState(&status);
        else {
            unrecognized();
            return;
        }

        Log.notice(S_F("%s : %S" NL), buffer, status ? TO_F(ON) : TO_F(OFF));
    }
    else if(BUFFER_IS_P(ON)) {
        readNext();

        if(BUFFER_IS_P(MAIN)) {
            bool poweredOn = sim808.powerOnOff(true);
            Log.notice(S_F("powered on %S : %S" NL), TO_F(MAIN), poweredOn ? TO_F(YES) : TO_F(NO));
        } 
        else if(BUFFER_IS_P(GPS)) {
            bool poweredOn = sim808.powerOnOffGps(true);
            Log.notice(S_F("powered on %S : %S" NL), TO_F(GPS), poweredOn ? TO_F(YES) : TO_F(NO));
        }
        else if(BUFFER_IS_P(NETWORK)) {
            char apn[15], user[15], pass[15];
            char *userP = NULL, *passP = NULL;
            
            PRINT("[apn] [pass?=\"\"] [user?=\"\"] ?");
            bool last = readNext();
            strncpy(apn, buffer, 15);

            if(!last) {
                last = readNext();
                strncpy(user, buffer, 15);
                userP = *user;
            }

            if(!last) {
                last = readNext();
                strncpy(pass, buffer, 15);
                passP = *pass;
            }

            bool success = sim808.enableGprs(apn, userP, passP);
            Log.notice(S_F("enable GPRS : %S" NL), success ? TO_F(SUCCESS) : TO_F(FAILED));
        }
        else {
            unrecognized();
            return;
        }
    }
    else if(BUFFER_IS_P(OFF)) {
        readNext();

        if(BUFFER_IS_P(MAIN)) {
            bool poweredOff = sim808.powerOnOff(false);
            Log.notice(S_F("powered off %S : %S" NL), TO_F(MAIN), poweredOff ? TO_F(YES) : TO_F(NO));
        }
        else if(BUFFER_IS_P(GPS)) {
            bool poweredOff = sim808.powerOnOffGps(false);
            Log.notice(S_F("powered off %S : %S" NL), TO_F(GPS), poweredOff ? TO_F(YES) : TO_F(NO));

        }
        else if(BUFFER_IS_P(NETWORK)) sim808.disableGprs();
        else {
            unrecognized();
            return;
        }
    }
    else {
        unrecognized();
        return;
    }
}

void charge() {
    SIM808ChargingStatus status = sim808.getChargingState();
    __FlashStringHelper * state;

    switch(status.state) {
        case SIM808ChargingState::Error:
            state = TO_F(ERROR);
            break;
        case SIM808ChargingState::NotCharging:
            state = S_F("NotCharging");
            break;
        case SIM808ChargingState::Charging:
            state = S_F("Charging");
            break;
        case SIM808ChargingState::ChargingDone:
            state = S_F("ChargingDone");
            break;
        default:
            state = TO_F(UNKNOWN);
            break;
    }

    Log.notice(S_F("Charging status" NL));
    Log.notice(S_F("state\t: %S" NL), state);
    Log.notice(S_F("level\t: %d%%" NL), status.level);
    Log.notice(S_F("voltage\t: %dmV" NL), status.voltage);
}

void sim() {
    readNext();

    if(BUFFER_IS_P(STATUS)) {
        sim808.getSimState(buffer, BUFFER_SIZE);
        Log.notice(S_F("SIM status : \"%s\"" NL), buffer);
    }
    else if(BUFFER_IS("UNLOCK")) {
        readNext();
        
        size_t length = strlen(buffer);
        if(length != 4) {
            Log.error(S_F("4 digit pin code required" NL));
            return;
        }

        bool success = sim808.simUnlock(buffer);
        Log.notice(S_F("SIM unlock : %S" NL), success ? TO_F(SUCCESS) : TO_F(FAILED));
    }
    else {
        unrecognized();
        return;
    }
}

void imei() {
    sim808.getImei(buffer, BUFFER_SIZE);
    Log.notice(S_F("IMEI : \"%s\"" NL), buffer);
}

void network() {
    bool last = readNext();
    __FlashStringHelper * state;

    if(BUFFER_IS_P(STATUS)) {
        SIM808NetworkRegistrationState status = sim808.getNetworkRegistrationStatus();

        switch(status) {
            case SIM808NetworkRegistrationState::Error:
                state = TO_F(ERROR);
                break;
            case SIM808NetworkRegistrationState::NotSearching:
                state = S_F("NotSearching");
                break;
            case SIM808NetworkRegistrationState::Registered:
                state = S_F("Registered");
                break;
            case SIM808NetworkRegistrationState::Searching:
                state = S_F("Searching");
                break;
            case SIM808NetworkRegistrationState::Denied:
                state = S_F("Denied");
                break;
            case SIM808NetworkRegistrationState::Unknown:
                state = S_F("Unknown");
                break;
            case SIM808NetworkRegistrationState::Roaming:
                state = S_F("Roaming");
                break;
            default:
                state = TO_F(UNKNOWN);
                break;
        }

        Log.notice(S_F("status : %S" NL), state);

    }
    else if(BUFFER_IS("QUALITY")) {
        SIM808SignalQualityReport report = sim808.getSignalQuality();

        Log.notice(S_F("Signal Quality" NL));
        Log.notice(S_F("signal strength \t: %d" NL), report.rssi);
        Log.notice(S_F("bit error rate \t: %d" NL), report.ber);
        Log.notice(S_F("attenuation \t\t: %ddBm" NL), report.attenuation);
    }
    else if(BUFFER_IS("FUNCTIONALITY")) {
        if(last) { //get
            SIM808PhoneFunctionality fun = sim808.getPhoneFunctionality();
            switch(fun) {
                case SIM808PhoneFunctionality::Fail:
                    state = TO_F(FAILED);
                    break;
                case SIM808PhoneFunctionality::Minimum:
                    state = TO_F(MINIMUM);
                    break;
                case SIM808PhoneFunctionality::Full:
                    state = TO_F(FULL);
                    break;
                case SIM808PhoneFunctionality::Disabled:
                    state = TO_F(RF_DISABLED);
                    break;
                default:
                    state = TO_F(UNKNOWN);
                    break;
            }

            Log.notice(S_F("phone functionality : %S" NL), state);
        }
        else {
            SIM808PhoneFunctionality fun;

            readNext();
            if(BUFFER_IS_P(MINIMUM)) fun = SIM808PhoneFunctionality::Minimum;
            else if(BUFFER_IS_P(FULL)) fun = SIM808PhoneFunctionality::Full;
            else if(BUFFER_IS_P(RF_DISABLED)) fun = SIM808PhoneFunctionality::Disabled;
            else {
                unrecognized();
                return;
            }

            bool success = sim808.setPhoneFunctionality(fun);
            Log.notice(S_F("phone functionality : %S" NL), success ? TO_F(SUCCESS) : TO_F(FAILED));            
        }
    }
}

void gps() {
    char position[128];

    Log.notice(S_F("GPS" NL));
    readNext();

    if(BUFFER_IS("RAW") || BUFFER_IS("PARSE")) {
        sim808.getGpsPosition(position, 128);
        Log.notice(S_F("position\t\t: \"%s\"" NL), position);
    }
    
    if(BUFFER_IS("PARSE")) {
        char * timeStr;
        float tmpFloat;
        uint16_t tmpInt;
        bool oneField;

        SIM808GpsStatus status = sim808.getGpsStatus(position, 128);
        __FlashStringHelper * state;

        switch(status) {
            case SIM808GpsStatus::Fail:
                state = TO_F(FAILED);
                break;
            case SIM808GpsStatus::Off:
                state = TO_F(OFF);
                break;
            case SIM808GpsStatus::NoFix:
                state = S_F("NoFix");
                break;
            case SIM808GpsStatus::Fix:
                state = S_F("Fix");
                break;
            case SIM808GpsStatus::AccurateFix:
                state = S_F("AccurateFix");
                break;
            default:
                state = TO_F(UNKNOWN);
                break;
        }

        Log.notice(S_F("status\t\t: %S" NL), state);

        readNext();
        if(BUFFER_IS_P(ALL) || BUFFER_IS("TIME")) {
            oneField = true;
            sim808.getGpsField(position, SIM808GpsField::Utc, &timeStr);
            Log.notice(S_F("time\t\t\t: %s" NL), timeStr);
        }
        
        if(BUFFER_IS_P(ALL) || BUFFER_IS("LAT")) {
            oneField = true;
            sim808.getGpsField(position, SIM808GpsField::Latitude, &tmpFloat);
            Log.notice(S_F("latitude\t\t: %F" NL), tmpFloat);
        }

        if(BUFFER_IS_P(ALL) || BUFFER_IS("LON")) {
            oneField = true;
            sim808.getGpsField(position, SIM808GpsField::Longitude, &tmpFloat);
            Log.notice(S_F("longitude\t\t: %F" NL), tmpFloat);
        }

        if(BUFFER_IS_P(ALL) || BUFFER_IS("ALT")) {
            oneField = true;
            sim808.getGpsField(position, SIM808GpsField::Altitude, &tmpFloat);
            Log.notice(S_F("altitude\t\t: %F" NL), tmpFloat);
        }

        if(BUFFER_IS_P(ALL) || BUFFER_IS("SPEED")) {
            oneField = true;
            sim808.getGpsField(position, SIM808GpsField::Speed, &tmpFloat);
            Log.notice(S_F("speed\t\t: %F" NL), tmpFloat);
        }

        if(BUFFER_IS_P(ALL) || BUFFER_IS("COURSE")) {
            oneField = true;
            sim808.getGpsField(position, SIM808GpsField::Course, &tmpFloat);
            Log.notice(S_F("course\t\t: %F" NL), tmpFloat);
        }

        if(BUFFER_IS_P(ALL) || BUFFER_IS("SATVIEW")) {
            oneField = true;
            sim808.getGpsField(position, SIM808GpsField::GpsInView, &tmpInt);
            Log.notice(S_F("satellites in view\t: %d" NL), tmpInt);
        }

        if(BUFFER_IS_P(ALL) || BUFFER_IS("SATUSED")) {
            oneField = true;
            sim808.getGpsField(position, SIM808GpsField::GnssUsed, &tmpInt);
            Log.notice(S_F("satellites used\t: %d" NL), tmpInt);
        }

        if(!oneField) {
            unrecognized();
            return;
        }
    }
}

void sendHttp() {
    bool defaultUrl = readNext();
    char action[5];
    char url[50];

    strlcpy(action, buffer, 5);

    if(defaultUrl) strlcpy_P(url, DEFAULT_URL, 50);
    else {
        readNext();
        strlcpy(url, buffer, 50);
    }

    if(STRING_IS(action, "GET")) {
        uint16_t code = sim808.httpGet(url, buffer, BUFFER_SIZE);

        Log.notice(S_F("HTTP" NL));
        Log.notice(S_F("Server responded : %d" NL), code);
        Log.notice(S_F("\"%s\""), buffer);
    }
    else if(STRING_IS(action, "POST")) {
        PRINT("[body] ?");
        readNext(true);
        uint16_t code = sim808.httpPost(url, S_F("text/plain"), buffer, buffer, BUFFER_SIZE);

        Log.notice(S_F("HTTP" NL));
        Log.notice(S_F("Server responded : %d" NL), code);
        Log.notice(S_F("\"%s\""), buffer);
    }
    else {
        unrecognized();
        return;
    }
}

void send() {
     readNext();

    if(BUFFER_IS("SMS")) {
        char number[20];

        readNext();
        strncpy(number, buffer, 20);

        PRINT("[message] ?");
        readNext(true); //read the whole line into buffer
        bool success = sim808.sendSms(number, buffer);
        Log.notice(S_F("send SMS : %S" NL), success ? TO_F(SUCCESS) : TO_F(FAILED));
    }
    else if(BUFFER_IS("HTTP")) return sendHttp();
    else if(BUFFER_IS("AT")) {
        readNext(true);
        sim808.sendCommand(buffer, buffer, BUFFER_SIZE);

        Log.notice(S_F("response : %s" NL), buffer);
    }
    else {
        unrecognized();
        return;
    }
}

void setup() {
    while(!Serial);

    Serial.begin(SERIAL_BAUDRATE);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);

    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);

    Log.notice(S_F("Powering on SIM808..." NL));
    sim808.powerOnOff(true);
    sim808.init();

    usage();
}

void loop() {
    readNext();

    if(BUFFER_IS("?")) usage();
    else if(BUFFER_IS("reset")) reset();
    else if(BUFFER_IS("init")) init_();
    else if(BUFFER_IS("power")) power();
    else if(BUFFER_IS("charge")) charge();
    else if(BUFFER_IS("sim")) sim();
    else if(BUFFER_IS("imei")) imei();
    else if(BUFFER_IS("send")) send();
    else if(BUFFER_IS_P(NETWORK)) network();
    else if(BUFFER_IS_P(GPS)) gps();
    else {
        unrecognized();
        return;
    }

    Log.notice(S_F("Done" NL));
}
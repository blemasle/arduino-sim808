#include "MainExample.h"
#include "SIM808.h"

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
SIM808 sim = SIM808(FONA_RST);
bool first = true;
char buffer[512];

void sendPrintResponse(const char *cmd)
{
	char buffer[255];
	sim.sendCommand(cmd, buffer);
	Serial.print("Response :");
	Serial.println(buffer);
}

int freeRam()
{
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

// the setup function runs once when you press reset or power the board
void setup() {
	while (!Serial);
}

// the loop function runs over and over again until power down or reset
void loop() {

	if (!first) {
		delay(1000);
		return;
	}
	first = false;

	Serial.begin(115200);
	Serial.println("Initialization...");

	fonaSerial->begin(4800);
	sim.begin(*fonaSerial);

	sim.getImei(buffer);
	Serial.print("IMEI :");
	Serial.println(buffer);

	/*sim.enableGps();
	while (true)
	{
	delay(2000);
	sim.getGpsPosition(buffer);
	Serial.print(buffer);
	}*/

	Serial.println("Unlocking sim...");
	bool unlocked = sim.simUnlock("1234");
	Serial.println(unlocked);
	delay(5000);

	//sendPrintResponse("AT+CCLK?");
	/*sendPrintResponse("AT+CLTS=1");
	delay(5000);*/
	//sendPrintResponse("AT+CCLK?");

	/*bool sent = sim.sendSms("+33642559310", "Hello World !");
	Serial.print("Sent ");
	Serial.println(sent);*/

	//TODO before send an actual http request : context gprs, apn etc
	bool gprsEnabled = false;
	SIM808RegistrationStatus status;
	do
	{
		do {
			delay(2000);
			status = sim.getNetworkRegistrationStatus();
			Serial.print("Status : ");
			Serial.print(status.n);
			Serial.print(", ");
			Serial.println(status.stat);
		} while (status.stat != 3 && status.stat != 2 && status.stat != 5);
		gprsEnabled = sim.enableGprs("Free");
	} while (!gprsEnabled);

	status = sim.getNetworkRegistrationStatus();
	Serial.print("Status : ");
	Serial.print(status.n);
	Serial.print(", ");
	Serial.println(status.stat);

	/*while (true)
	{
	*/
	uint16_t code = sim.httpGet("http://httpbin.org/anything", buffer, 512);
	Serial.print("Server responded ");
	Serial.println(code);
	Serial.print(buffer);

	code = sim.httpPost("http://httpbin.org/anything", "text/plain", "Hello world !", buffer, 512);
	Serial.print("Server responded ");
	Serial.println(code);
	Serial.print(buffer);
	//}
}

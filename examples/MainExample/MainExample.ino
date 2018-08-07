#include "MainExample.h"
#include "SIM808.h"

SoftwareSerial simSerial = SoftwareSerial(7, 6);
SIM808 sim = SIM808(5, 9, 8);
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

bool isAvailable(SIM808_NETWORK_REGISTRATION_STATE state) {
	return static_cast<int8_t>(state) &
		(static_cast<int8_t>(SIM808_NETWORK_REGISTRATION_STATE::REGISTERED) | static_cast<int8_t>(SIM808_NETWORK_REGISTRATION_STATE::ROAMING))
		!= 0;
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

	simSerial.begin(4800);
	sim.begin(simSerial);
	sim.powerOnOff(true);
	sim.init();
	
	sim.getImei(buffer);
	Serial.print("IMEI :");
	Serial.println(buffer);

	sim.sendCommand("ATI", buffer);
	Serial.print("ATI :");
	Serial.println(buffer);
	/*sim.enableGps();
	while (true)
	{
	delay(2000);
	sim.getGpsPosition(buffer);
	Serial.print(buffer);
	}*/

	// Serial.println("Unlocking sim...");
	// bool unlocked = sim.simUnlock("1234");
	// Serial.println(unlocked);
	// delay(5000);

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
			Serial.println((uint8_t)status.stat);
		} while (!isAvailable(status.stat));
		gprsEnabled = sim.enableGprs("Vodafone");
	} while (!gprsEnabled);

	uint16_t code = sim.httpGet("https://httpbin.org/anything", buffer, 512);
	Serial.print("Server responded ");
	Serial.println(code);
	Serial.print(buffer);

	/*while (true)
	{
	*/
	// uint16_t code = sim.httpGet("https://httpbin.org/anything", buffer, 512);
	// Serial.print("Server responded ");
	// Serial.println(code);
	// Serial.print(buffer);

	// code = sim.httpPost("https://httpbin.org/anything", F("text/plain"), "Hello world !", buffer, 512);
	// Serial.print("Server responded ");
	// Serial.println(code);
	// Serial.print(buffer);
	//}
}

#include "WProgram.h"
#include "arduino-xbee/Series1XBee.h"
#include "arduino-xbee/NewSoftSerialApiModeXBee.h"
//#include "OnOffLightService.h"
//#include "Avieul.h"

// general definitions
extern "C" void __cxa_pure_virtual(void);
void __cxa_pure_virtual(void) {}

void * operator new(size_t size);
void * operator new(size_t size) {
   return malloc(size);
}
// end of general definitions


#define DEBUG_PRINTS 1


// prototypes
void blink(int n, int d);

// variables
int ledPin = 13; // LED connected to digital pin 13
int xbeeRcvPin = 2;
int xbeeSendPin = 3;

NewSoftSerial *serial;
LowlevelXBee *lowlevel;
Series1XBee *xbee;
//Avieul *avieul;

void setup() {
#ifdef DEBUG_PRINTS
	Serial.begin(9600);
#endif

	serial = new NewSoftSerial(xbeeRcvPin, xbeeSendPin);
	serial->begin(19200);
	lowlevel = new NewSoftSerialApiModeXBee(serial);
	xbee = new Series1XBee(lowlevel);
/*
	OnOffLightService *onOff = new OnOffLightService(13);
	AvieulService* services[] = { onOff };
	avieul = new Avieul(xbee, services, 1);
*/
	pinMode(ledPin, OUTPUT); // sets the digital pin as output
	digitalWrite(ledPin, HIGH);

#ifdef DEBUG_PRINTS
	Serial.println("Started");
#endif
}


void printHex(uint8_t *data, uint8_t len, bool seperate);
void printAddress(uint64_t address);

void loop() {
	bool oldVal = digitalRead(ledPin) == HIGH;
	digitalWrite(ledPin, oldVal ? LOW : HIGH);

	if (xbee->available()) {
		XBeeAddress address;
		uint8_t *data;
		uint8_t len;
		xbee->receive(&address, &data, &len);

		Serial.print("incoming message '");
		printHex(data, len, true);
		Serial.print("' from ");
		printAddress(address);
		Serial.println();
	}

	delay(500);
}






int main(void) {
	init();
	setup();
	for (;;) loop();
	return 0;
}

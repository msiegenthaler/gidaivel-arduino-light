#include "WProgram.h"
#include "arduino-xbee/Series1XBee.h"
#include "arduino-xbee/ApiModeXBee.h"
#include "arduino-xbee/NewSoftSerial/NewSoftSerial.h"
#include "OnOffLightService.h"
#include "Avieul.h"

// general definitions
extern "C" void __cxa_pure_virtual(void);
void __cxa_pure_virtual(void) {}

// New and delete
void * operator new(size_t size);
void operator delete(void * ptr);
void * operator new(size_t size) {
   return malloc(size);
}
void operator delete(void * ptr) {
	//TODO call destructor?
	free(ptr);
}
// end of general definitions


#define DEBUG_PRINTS 1

// prototypes
void blink(int n, int d);

// variables
int ledPin = 13; // LED connected to digital pin 13
int xbeeRcvPin = 8;
int xbeeSendPin = 9;

NewSoftSerial serial = NewSoftSerial(xbeeRcvPin, xbeeSendPin, false);
ApiModeXBee lowlevel = ApiModeXBee(&serial);
Series1XBee xbee = Series1XBee(&lowlevel);
OnOffLightService onOff = OnOffLightService(13);
AvieulService* services[] = { &onOff };
Avieul avieul = Avieul(&xbee, services, 1);

void setup() {
#ifdef DEBUG_PRINTS
	Serial.begin(9600);
#endif

	pinMode(ledPin, OUTPUT); // sets the digital pin as output

#ifdef DEBUG_PRINTS
	Serial.println("Started");
#endif
}

void loop() {
	avieul.process();

#ifdef DEBUG_PRINTS
	Serial.println("Loop");
#endif
}






int main(void) {
	init();
	setup();
	for (;;) loop();
	return 0;
}

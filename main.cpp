#include "WProgram.h"
#include "arduino-xbee/Series1XBee.h"
#include "arduino-xbee/ApiModeXBee.h"
#include "arduino-xbee/NewSoftSerial/NewSoftSerial.h"

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
	byte data[] = {1, 2, 3, 4, 5};
	xbee.send(123L, data, 5);

#ifdef DEBUG_PRINTS
	Serial.println("Sent something");
#endif

	int d = 1000;
	digitalWrite(ledPin, HIGH);
	delay(d / 2);
	digitalWrite(ledPin, LOW);
	delay(d / 2);
}






int main(void) {
	init();
	setup();
	for (;;) loop();
	return 0;
}

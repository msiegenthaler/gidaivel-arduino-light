#include "OnOffLightService.h"

OnOffLightService::OnOffLightService(int pin) {
	_type = 0x12;
	_version = 0x01;
	_pin = pin;
}

bool OnOffLightService::processRequest(uint16_t requestType, XBeeAddress from, uint8_t* payload, uint8_t payload_length) {
	if (requestType == 0x01) {
		//is on
		uint8_t value = currentValue() ? 0x01 : 0x00;
		uint8_t data[] = { 0, 0, 0, 0, value };
		fillResponseHeader(data, requestType);
		_sender->send(from, data, 5);
#ifdef DEBUG_ASRVS
		Serial.println("OOL-srv: get light state");
#endif
		return true;
	} else if (requestType == 0x02) {
		//set value
		bool on = payload[0] == 0x00;
		setValue(on);
		uint8_t data[] = { 0, 0, 0, 0, on ? 0x01 : 0x00 };
		fillResponseHeader(data, requestType);
		_sender->send(from, data, 5);
#ifdef DEBUG_ASRVS
		Serial.print("OOL-srv: set light to ");
		Serial.println(on ? "on" : "off");
#endif
		return true;
	} else if (requestType == 0x03) {
		//switch value
		bool on = !currentValue();
		setValue(on);
		uint8_t data[] = { 0, 0, 0, 0, on ? 0x01 : 0x00 };
		fillResponseHeader(data, requestType);
		_sender->send(from, data, 5);
#ifdef DEBUG_ASRVS
		Serial.print("OOL-srv: switch light to ");
		Serial.println(on ? "on" : "off");
#endif
		return true;
	} else {
#ifdef DEBUG_ASRVS
		Serial.print("OOL-srv: unknown request 0x");
		Serial.println(requestType, HEX);
#endif
		return false;
	}
}

bool OnOffLightService::currentValue() {
	return digitalRead(_pin) == HIGH;
}

void OnOffLightService::setValue(bool on) {
	digitalWrite(_pin, on ? HIGH : LOW);
}

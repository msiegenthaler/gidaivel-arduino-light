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
		return true;
	} else if (requestType == 0x02) {
		//set value
		bool on = payload[0] == 0x00;
		setValue(on);
		uint8_t data[] = { 0, 0, 0, 0, on ? 0x01 : 0x00 };
		fillResponseHeader(data, requestType);
		_sender->send(from, data, 5);
		return true;
	} else if (requestType == 0x03) {
		//switch value
		bool on = !currentValue();
		setValue(on);
		uint8_t data[] = { 0, 0, 0, 0, on ? 0x01 : 0x00 };
		fillResponseHeader(data, requestType);
		_sender->send(from, data, 5);
		return true;
	} else return false;
}

bool OnOffLightService::currentValue() {
	return digitalRead(_pin) == HIGH;
}

void OnOffLightService::setValue(bool on) {
	digitalWrite(_pin, on ? HIGH : LOW);
}

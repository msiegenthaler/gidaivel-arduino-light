#include "OnOffLightService.h"

OnOffLightService::OnOffLightService(int pin) {
	_type = 0x12;
	_version = 0x00;
	_pin = pin;
}

void OnOffLightService::processCall(uint16_t callType, XBeeAddress from, uint8_t* payload, uint8_t payload_length) {
	switch (callType) {
	case 0x0001: { //set on/off
		bool on = payload[0] == 0x01;
		setValue(on);
		break;
	}

	default: break;
	}
}

bool OnOffLightService::processRequest(uint16_t requestType, XBeeAddress from, uint8_t* payload, uint8_t payload_length) {
	switch (requestType) {
	case 0x0001: { //is on?
		uint8_t value = currentValue() ? 0x01 : 0x00;
		uint8_t data[] = { 0, 0, 0, 0, value };
		fillResponseHeader(data, requestType);
		_sender->send(from, data, 5);
		return true;
	}
	default: return false;
	}
}

bool OnOffLightService::addSubscription(XBeeAddress from, uint16_t subscriptionType) {
	switch (subscriptionType) {
	case 0x0001: // on/off changes
		addSubscriber(from);
		return true;

	default: return false;
	}
}
void OnOffLightService::removeSubscription(XBeeAddress from, uint16_t subscriptionType) {
	switch (subscriptionType) {
	case 0x0001: //on/off change
		removeSubscriber(from);
		break;

	default: break;
	}
}

inline bool exists(XBeeAddress *all, uint8_t count, XBeeAddress toFind) {
	for (uint8_t i=0; i<count; i++) {
		if (all[i] == toFind) return true;
	}
	return false;
}

void OnOffLightService::addSubscriber(XBeeAddress from) {
	if (exists(_subscribers, _subscriber_count, from)) return; //already subscribed

	size_t size = sizeof(XBeeAddress) * (_subscriber_count + 1);
	XBeeAddress* na = (XBeeAddress*)malloc(size);
	memcpy(na, _subscribers, size);
	na[_subscriber_count] = from;
	free(_subscribers);
	_subscribers = na;
	_subscriber_count++;
}

void OnOffLightService::removeSubscriber(XBeeAddress from) {
	if (!exists(_subscribers, _subscriber_count, from)) return; //not subscribed

	size_t size = sizeof(XBeeAddress) * (_subscriber_count - 1);
	XBeeAddress* na = (XBeeAddress*)malloc(size);
	uint8_t c=0;
	for (uint8_t i=0; i<_subscriber_count; i++) {
		if (_subscribers[i] == from) break;
		na[c++] = _subscribers[i];
	}
	free(_subscribers);
	_subscribers = na;
	_subscriber_count = c;
}

void OnOffLightService::notifySubscribers() {
	uint8_t data[5]; // 4 + 1
	fillPublishHeader(data, 0x0001);
	data[4] = currentValue() ? 0x1 : 0x0;

	for (uint8_t i=0; i < _subscriber_count; i++) {
		_sender->send(_subscribers[i], data, 5);
	}
}


bool OnOffLightService::currentValue() {
	return digitalRead(_pin) == HIGH;
}

void OnOffLightService::setValue(bool on) {
	digitalWrite(_pin, on ? HIGH : LOW);
	notifySubscribers();
}

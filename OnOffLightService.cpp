#include "OnOffLightService.h"


OnOffLightService::OnOffLightService(int pin) {
	_type = 0x12;
	_version = 0x00;
	_pin = pin;
	_subscription = new SubscriptionManager(_sender);
}

void OnOffLightService::processCall(uint16_t callType, XBeeAddress from, uint8_t* payload, uint8_t payload_length) {
	switch (callType) {
	case 0x0001: { //set on/off
		bool on = payload[0] == 0x01;
#ifdef DEBUG_ASRVS
		Serial.print("Turn light to ");
		Serial.println(on);
#endif
		setValue(on);
		break;
	}

	default: break;
	}
}

bool OnOffLightService::processRequest(uint16_t requestType, XBeeAddress from, uint8_t* payload, uint8_t payload_length) {
	switch (requestType) {
	case 0x0001: { //is on?
#ifdef DEBUG_ASRVS
		Serial.println("Request for current status");
#endif
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
		_subscription->add(from);
		return true;

	default: return false;
	}
}
void OnOffLightService::removeSubscription(XBeeAddress from, uint16_t subscriptionType) {
	switch (subscriptionType) {
	case 0x0001: //on/off change
		_subscription->remove(from);
		break;

	default: break;
	}
}


void OnOffLightService::stateChanged() {
	uint8_t data[5]; // 4 + 1
	fillPublishHeader(data, 0x0001);
	data[4] = currentValue() ? 0x1 : 0x0;
	_subscription->publish(data, 5);
}


bool OnOffLightService::currentValue() {
	return digitalRead(_pin) == HIGH;
}

void OnOffLightService::setValue(bool on) {
	digitalWrite(_pin, on ? HIGH : LOW);
	stateChanged();
}

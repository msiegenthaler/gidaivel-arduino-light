#import "Avieul.h"


//AvieulService::AvieulService(uint32_t type, uint8_t version) {
//	_type = type;
//	_version = version;
//}

void AvieulService::initialize(uint8_t index, AvieulSender *sender) {
	_index = index;
	_sender = sender;
}

uint32_t AvieulService::getType() {
	return _type;
}
uint8_t AvieulService::getVersion() {
	return _version;
}
uint8_t AvieulService::getIndex() {
	return _index;
}

uint16_t parseType(uint8_t *data) {
	uint16_t res = data[0];
	res = (res << 8) | data[1];
	return res;
}
uint8_t offset(uint8_t offset) {
	return offset * sizeof(uint8_t);
}

void AvieulService::process(XBeeAddress from, uint8_t* request, uint8_t request_length) {
	if (request_length<2) return;
	uint8_t messageType = request[0];
	if (messageType == 0x20) {
		//call
		if (request_length<4) return;
		uint16_t callType = parseType(request + offset(2));
		processCall(callType, from, request + offset(4), request_length - offset(4));
	} else if (messageType == 0x30) {
		//request
		if (request_length<4) return;
		uint16_t requestType = parseType(request + offset(2));
		if (!processRequest(requestType, from, request + offset(4), request_length - offset(4))) {
			uint8_t data[] = { 0x3F, _index, request[2], request[3]};
			_sender->send(from, data, 4);
		}

	} else if (messageType == 0x40) {
		//subscribe
		if (request_length<4) return;
		uint16_t subscriptionType = parseType(request + offset(2));
		if (!addSubscription(from, subscriptionType)) {
			uint8_t data[] = { 0x4F, _index, request[2], request[3]};
			_sender->send(from, data, 4);
		}
	} else if (messageType == 0x41) {
		//unsubscribe
		if (request_length<4) return;
		uint16_t subscriptionType = parseType(request + offset(2));
		removeSubscription(from, subscriptionType);
	} else {
		//unknown request
	}
}

void AvieulService::fillResponseHeader(uint8_t *buffer, uint16_t requestType) {
	buffer[0] = 0x31;
	buffer[1] = _index;
	buffer[2] = (requestType >> 8) & 0xFF;
	buffer[3] = requestType & 0xFF;
}

void AvieulService::processCall(uint16_t callType, XBeeAddress from, uint8_t* payload, uint8_t payload_length) {
}
bool AvieulService::processRequest(uint16_t requestType, XBeeAddress from, uint8_t* payload, uint8_t payload_length) {
	return false;
}
bool AvieulService::addSubscription(XBeeAddress from, uint16_t subscriptionType) {
	return false;
}
void AvieulService::removeSubscription(XBeeAddress from, uint16_t subscriptionType) {
}



Avieul::Avieul(Series1XBee *xbee, AvieulService **services, uint8_t service_count) {
	_xbee = xbee;
	_services_count = service_count;
	_services = (AvieulService**)malloc(sizeof(AvieulService*) * service_count);
	for (int i=0; i<_services_count; i++) {
		_services[i] = services[i];
		_services[i]->initialize(i, this);
	}
	announce(BROADCAST_ADDRESS);
}

void Avieul::send(XBeeAddress to, uint8_t* data, uint8_t data_length) {
	_xbee->send(to, data, data_length);
}

void Avieul::handle(XBeeAddress from, uint8_t* data, uint8_t data_length) {
	if (data_length < 1) return;

	uint8_t messageType = data[0];
	if (messageType < 0x10) {
		//General messages
		if (messageType == 0x01) { //RequestInfo
			//Announce us

		} else if (messageType == 0x02) { //AnnounceService
			//ignore announcements
		} else {
			//unknown request, ignore
		}
	} else {
		if (isBroadcastAddress(from)) return; //do not accept broadcasts

		//Service messages
		if (data_length < 2) return;
		uint8_t serviceIndex = data[1];
		if (serviceIndex >= _services_count) {
			//call to an unknown services
			uint8_t data[] = {0x10, serviceIndex}; //UnknownService
			_xbee->send(from, data, 2);
			return;
		} else {
			//forward call to the appropriate service
			_services[serviceIndex]->process(from, data, data_length);
		}
	}
}

void Avieul::announce(XBeeAddress to) {
	uint8_t size = 1 + 3*_services_count;
	uint8_t *data = (uint8_t*) malloc(size * sizeof(uint8_t));
	data[0] = 0x02;
	for (uint8_t i=0; i<_services_count; i++) {
		AvieulService *service = _services[i];
		uint32_t type = service->getType();
		data[i*6+1] = i;                     //index
		data[i*6+2] = (type >> 24) & 0xFF;   // ||
		data[i*6+3] = (type >> 16) & 0xFF;   //  || type
		data[i*6+4] = (type >> 8) & 0xFF;    //  ||
		data[i*6+5] = type & 0xFF;           // ||
		data[i*6+6] = service->getVersion(); // version
	}
	_xbee->send(to, data, size);
	free(data);
}

void Avieul::process() {
	if (_xbee->available()) {
		XBeeAddress from = 0;
		uint8_t length = 0;
		_xbee->receive(&from, (uint8_t**)&_buffer, &length);
		handle(from, _buffer, length);
	}
}

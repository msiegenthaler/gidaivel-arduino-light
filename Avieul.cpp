#import "Avieul.h"


AvieulService::AvieulService(uint64_t type) {
	_type = type;
}

void AvieulService::initialize(uint8_t index, AvieulSender *sender) {
	_index = index;
	_sender = sender;
}

uint64_t AvieulService::getType() {
	return _type;
}
uint8_t AvieulService::getIndex() {
	return _index;
}


Avieul::Avieul(Series1XBee *xbee, AvieulService **services, uint8_t service_count) {
	_xbee = xbee;
	_services_count = service_count;
	_services = (AvieulService**)malloc(sizeof(AvieulService*) * service_count);
	for (int i=0; i<_services_count; i++) {
		_services[i] = services[i];
		_services[i]->initialize(i, this);
	}
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
			byte data[] = {0x10, serviceIndex}; //UnknownService
			_xbee->send(from, data, data.length);
			return;
		} else {
			//forward call to the appropriate service
			_services[serviceIndex]->process(from, data, data_length);
		}
	}
}

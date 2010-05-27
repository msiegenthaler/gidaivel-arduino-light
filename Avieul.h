#ifndef AVIEUL_H_
#define AVIEUL_H_

#include "WConstants.h"
#import "arduino-xbee/Series1XBee.h"


class AvieulService;

class AvieulSender {
	virtual void send(XBeeAddress to, uint8_t* data, uint8_t data_length);
};


/**
 * An Avieul-service.
 * Responsible for processing incoming commands and executing the desired functions.
 */
class AvieulService {
public:
	AvieulService(uint64_t type);
	void initialize(uint8_t index, AvieulSender *sender);

	uint64_t getType();
	uint8_t getIndex();

	/**
	 * Process a call/request/subscription for this service.
	 */
	void process(XBeeAddress from, uint8_t* request, uint8_t request_length);

private:
	uint64_t _type;
	uint8_t _index;
	AvieulSender *_sender;
};



/**
 * Takes care of announcing and managing the services and forwards the incoming requests
 * to the services.
 */
class Avieul : public AvieulSender {
public:
	Avieul(Series1XBee *xbee, AvieulService **services, uint8_t service_count);

	void handle(XBeeAddress from, uint8_t* data, uint8_t data_length);
	void send(XBeeAddress to, uint8_t* data, uint8_t data_length);
private:
	Series1XBee *_xbee;
	AvieulService **_services;
	uint8_t _services_count;
};


#endif /* AVIEUL_H_ */

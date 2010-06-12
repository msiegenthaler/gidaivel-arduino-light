#ifndef ONOFFLIGHTSERVICE_H_
#define ONOFFLIGHTSERVICE_H_

#include "Avieul.h"

#define DEBUG_ASRVS

/**
 * Light that can be turned on or off.
 */
class OnOffLightService : public AvieulService {
public:
	OnOffLightService(int pin);

protected:
	virtual bool processRequest(uint16_t requestType, XBeeAddress from, uint8_t* payload, uint8_t payload_length);

	bool currentValue();
	void setValue(bool on);

private:
	int _pin;
};

#endif /* ONOFFLIGHTSERVICE_H_ */

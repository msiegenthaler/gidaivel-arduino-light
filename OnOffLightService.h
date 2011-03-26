#ifndef ONOFFLIGHTSERVICE_H_
#define ONOFFLIGHTSERVICE_H_

#include "../gidaivel-arduino-base/Avieul.h"
#include "../gidaivel-arduino-base/SubscriptionManager.h"

#define DEBUG_ASRVS

/**
 * Light that can be turned on or off.
 */
class OnOffLightService : public AvieulService {
public:
	OnOffLightService(int pin);

protected:
	virtual void processCall(uint16_t callType, XBeeAddress from, uint8_t* payload, uint8_t payload_length);
	virtual bool processRequest(uint16_t requestType, XBeeAddress from, uint8_t* payload, uint8_t payload_length);
	virtual bool addSubscription(XBeeAddress from, uint16_t subscriptionType);
	virtual void removeSubscription(XBeeAddress from, uint16_t subscriptionType);

	bool currentValue();
	void setValue(bool on);

private:
	inline void stateChanged();

private:
	int _pin;
	SubscriptionManager *_subscription;
};

#endif /* ONOFFLIGHTSERVICE_H_ */

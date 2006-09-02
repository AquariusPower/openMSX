// $Id$

#include "IconStatus.hh"
#include "EventDistributor.hh"
#include "Timer.hh"
#include "checked_cast.hh"
#include <cassert>

namespace openmsx {

IconStatus::IconStatus(EventDistributor& eventDistributor_)
	: eventDistributor(eventDistributor_)
{
	unsigned long long now = Timer::getTime();
	for (int i = 0; i < LedEvent::NUM_LEDS; ++i) {
		iconStatus[i] = false;
		iconTime[i] = now;
	}

	eventDistributor.registerEventListener(OPENMSX_LED_EVENT, *this);
}

IconStatus::~IconStatus()
{
	eventDistributor.unregisterEventListener(OPENMSX_LED_EVENT, *this);
}

bool IconStatus::getStatus(int icon) const
{
	return iconStatus[icon];
}

unsigned long long IconStatus::getTime(int icon) const
{
	return iconTime[icon];
}

bool IconStatus::signalEvent(shared_ptr<const Event> event)
{
	const LedEvent& ledEvent = checked_cast<const LedEvent&>(*event);
	LedEvent::Led led = ledEvent.getLed();
	bool status = ledEvent.getStatus();
	if (status != iconStatus[led]) {
		iconStatus[led] = status;
		iconTime[led] = Timer::getTime();
	}
	return true;
}

} // namespace openmsx

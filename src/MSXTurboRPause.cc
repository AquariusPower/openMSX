// $Id$

#include "MSXTurboRPause.hh"
#include "LedEvent.hh"
#include "EventDistributor.hh"
#include "MSXMotherBoard.hh"
#include "BooleanSetting.hh"

namespace openmsx {

MSXTurboRPause::MSXTurboRPause(MSXMotherBoard& motherBoard,
                               const XMLElement& config, const EmuTime& time)
	: MSXDevice(motherBoard, config, time)
	, pauseSetting(new BooleanSetting(motherBoard.getCommandController(),
	               "turborpause", "status of the TurboR pause", false))
	, status(255)
	, pauseLed(false)
	, turboLed(false)
	, hwPause(false)
{
	pauseSetting->attach(*this);
	reset(time);
}

MSXTurboRPause::~MSXTurboRPause()
{
	pauseSetting->detach(*this);
}

void MSXTurboRPause::reset(const EmuTime& time)
{
	pauseSetting->setValue(false);
	writeIO(0, 0, time);
}

void MSXTurboRPause::powerDown(const EmuTime& time)
{
	writeIO(0, 0, time);
}

byte MSXTurboRPause::readIO(word port, const EmuTime& time)
{
	return peekIO(port, time);
}

byte MSXTurboRPause::peekIO(word /*port*/, const EmuTime& /*time*/) const
{
	return pauseSetting->getValue() ? 1 : 0;
}

void MSXTurboRPause::writeIO(word /*port*/, byte value, const EmuTime& /*time*/)
{
	status = value;
	bool newTurboLed = (status & 0x80);
	if (newTurboLed != turboLed) {
		turboLed = newTurboLed;
		getMotherBoard().getEventDistributor().distributeEvent(
			EventDistributor::EventPtr(
				new LedEvent(LedEvent::TURBO, turboLed)));
	}
	updatePause();
}

void MSXTurboRPause::update(const Setting& /*setting*/)
{
	updatePause();
}

void MSXTurboRPause::updatePause()
{
	bool newHwPause = (status & 0x02) && pauseSetting->getValue();
	if (newHwPause != hwPause) {
		hwPause = newHwPause;
		if (hwPause) {
			getMotherBoard().pause();
		} else {
			getMotherBoard().unpause();
		}
	}

	bool newPauseLed = (status & 0x01) || hwPause;
	if (newPauseLed != pauseLed) {
		pauseLed = newPauseLed;
		getMotherBoard().getEventDistributor().distributeEvent(
			EventDistributor::EventPtr(
				new LedEvent(LedEvent::PAUSE, pauseLed)));
	}
}

} // namespace openmsx

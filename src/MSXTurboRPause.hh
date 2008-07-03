// $Id$

/*
 * This class implements the 2 Turbo-R specific LEDS:
 *
 * Bit 0 of IO-port 0xA7 turns the PAUSE led ON (1) or OFF (0)
 * Bit 7                           TURBO
 * TODO merge doc below
 */
#ifndef TURBORPAUSE_HH
#define TURBORPAUSE_HH

#include "MSXDevice.hh"
#include "Observer.hh"
#include <memory>

namespace openmsx {

class BooleanSetting;
class Setting;

/**
 * This class implements the MSX Turbo-R pause key
 *
 *  Whenever the pause key is pressed a flip-flop is toggled.
 *  The status of this flip-flop can be read from io-port 0xA7.
 *   bit 0 indicates the status (1 = pause active)
 *   all other bits read 0
 */
class MSXTurboRPause : public MSXDevice, private Observer<Setting>
{
public:
	MSXTurboRPause(MSXMotherBoard& motherBoard, const XMLElement& config);
	virtual ~MSXTurboRPause();

	virtual void reset(const EmuTime& time);
	virtual void powerDown(const EmuTime& time);

	virtual byte readIO(word port, const EmuTime& time);
	virtual byte peekIO(word port, const EmuTime& time) const;
	virtual void writeIO(word port, byte value, const EmuTime& time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);


private:
	// Observer<Setting>
	void update(const Setting& setting);

	void updatePause();

	const std::auto_ptr<BooleanSetting> pauseSetting;
	byte status;
	bool pauseLed;
	bool turboLed;
	bool hwPause;
};

REGISTER_MSXDEVICE(MSXTurboRPause, "TurboRPause");

} // namespace openmsx

#endif

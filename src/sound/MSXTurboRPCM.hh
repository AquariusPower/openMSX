// $Id$

#ifndef MSXTURBORPCM_HH
#define MSXTURBORPCM_HH

#include "MSXDevice.hh"
#include "Clock.hh"
#include "AudioInputConnector.hh"
#include <memory>

namespace openmsx {

class MSXMixer;
class DACSound8U;

class MSXTurboRPCM : public MSXDevice, private AudioInputConnector
{
public:
	MSXTurboRPCM(MSXMotherBoard& motherBoard, const XMLElement& config,
	             const EmuTime& time);
	virtual ~MSXTurboRPCM();

	virtual void reset(const EmuTime& time);
	virtual byte readIO(word port, const EmuTime& time);
	virtual byte peekIO(word port, const EmuTime& time) const;
	virtual void writeIO(word port, byte value, const EmuTime& time);

private:
	byte getSample(const EmuTime& time) const;
	bool getComp(const EmuTime& time) const;
	void hardwareMute(bool mute);

	MSXMixer& mixer;
	std::auto_ptr<DACSound8U> dac;
	Clock<15750> reference;
	byte DValue;
	byte status;
	byte hold;
	bool hwMute;
};

} // namespace openmsx

#endif

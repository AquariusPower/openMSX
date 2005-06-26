// $Id$

#include "SoundDevice.hh"
#include "XMLElement.hh"

using std::string;

namespace openmsx {

SoundDevice::SoundDevice(Mixer& mixer_)
	: mixer(mixer_)
{
	muted = true;
}

SoundDevice::~SoundDevice()
{
}

void SoundDevice::setMute(bool muted)
{
	this->muted = muted;
}

bool SoundDevice::isMuted() const
{
	return muted;
}

void SoundDevice::registerSound(const XMLElement& config,
                                Mixer::ChannelMode mode)
{
	const XMLElement& soundConfig = config.getChild("sound");
	short volume = soundConfig.getChildDataAsInt("volume");
	if (mode != Mixer::STEREO) {
		string modeStr = soundConfig.getChildData("mode", "mono");
		if (modeStr == "left") {
			mode = Mixer::MONO_LEFT;
		} else if (modeStr == "right") {
			mode = Mixer::MONO_RIGHT;
		} else {
			mode = Mixer::MONO;
		}
	}
	mixer.registerSound(*this, volume, mode);
}

void SoundDevice::unregisterSound()
{
	mixer.unregisterSound(*this);
}

} // namespace openmsx

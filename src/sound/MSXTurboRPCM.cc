// $Id$

#include "MSXTurboRPCM.hh"
#include "MSXMotherBoard.hh"
#include "AudioInputConnector.hh"
#include "DACSound8U.hh"
#include "MSXMixer.hh"
#include "serialize.hh"

namespace openmsx {

MSXTurboRPCM::MSXTurboRPCM(MSXMotherBoard& motherBoard,
                           const XMLElement& config)
	: MSXDevice(motherBoard, config)
	, mixer(motherBoard.getMSXMixer())
	, connector(new AudioInputConnector(
              motherBoard.getPluggingController(), "pcminput"))
	, dac(new DACSound8U(mixer, "PCM", "Turbo-R PCM", config))
	, reference(getCurrentTime())
	, hwMute(false)
{
	reset(getCurrentTime());
}

MSXTurboRPCM::~MSXTurboRPCM()
{
	hardwareMute(false);
}

void MSXTurboRPCM::reset(const EmuTime& time)
{
	reference.advance(time);
	status = 0;
	dac->reset(time);
	hardwareMute(false);
}

byte MSXTurboRPCM::readIO(word port, const EmuTime& time)
{
	return peekIO(port, time);
}

byte MSXTurboRPCM::peekIO(word port, const EmuTime& time) const
{
	byte result;
	switch (port & 0x01) {
	case 0:
		// bit 0-1  15.75kHz counter
		// bit 2-7  not used
		result = reference.getTicksTill(time) & 0x03;
		break;
	case 1:
		// bit 0   BUFF  0->D/A    TODO check this bit
		//               1->A/D
		// bit 1   MUTE  mute ALL sound  0->muted
		// bit 2   FILT  filter  0->standard signal
		//                       1->filtered signal
		// bit 3   SEL   select 0->D/A
		//                      1->Mic/Jack
		// bit 4   SMPL  sample/hold  0->sample
		//                            1->hold
		// bit 5-6       not used
		// bit 7   COMP  comparator result 0->greater
		//                                 1->smaller
		result = (getComp(time) ? 0x80 : 0x00) | (status & 0x1F);
		break;
	default: // unreachable, avoid warning
		assert(false);
		result = 0;
	}
	//PRT_DEBUG("PCM: read " << hex << (int)port << " " << (int)result << dec);
	return result;
}

void MSXTurboRPCM::writeIO(word port, byte value, const EmuTime& time)
{
	//PRT_DEBUG("PCM: write " << hex << (int)port << " " << (int)value << dec);
	switch (port & 0x01) {
	case 0:
		// While playing: sample value
		//       recording: compare value
		// Resets counter
		reference.advance(time);
		DValue = value;
		if (status & 0x02) {
			dac->writeDAC(DValue, time);
		}
		break;

	case 1:
		// bit 0   BUFF
		// bit 1   MUTE  mute _all_ sound  0->no sound
		// bit 2   FILT  filter  1->filter on
		// bit 3   SEL   select  1->Mic/Jack  0->D/A
		// bit 4   SMPL  sample/hold  1->hold
		// bit 5-7  not used
		byte change = status ^ value;
		status = value;

		if ((change & 0x01) && ((status & 0x01) == 0)) {
			dac->writeDAC(DValue, time);
		}
		// TODO status & 0x08
		if ((change & 0x10) && (status & 0x10)) {
			hold = getSample(time);
		}
		hardwareMute(!(status & 0x02));
		break;
	}
}

byte MSXTurboRPCM::getSample(const EmuTime& time) const
{
	byte result;
	if (status & 0x04) {
		result = (connector->readSample(time) / 256) + 0x80;
	} else {
		result = 0x80;	// TODO check
	}
	//PRT_DEBUG("PCM: read " << (int)result);
	return result;
}

bool MSXTurboRPCM::getComp(const EmuTime& time) const
{
	// TODO also when D/A ??
	byte sample = (status & 0x10) ? hold : getSample(time);
	return sample >= DValue;
}

void MSXTurboRPCM::hardwareMute(bool mute)
{
	if (mute ^ hwMute) {
		hwMute = mute;
		if (hwMute) {
			mixer.mute();
		} else {
			mixer.unmute();
		}
	}
}


template<typename Archive>
void MSXTurboRPCM::serialize(Archive& ar, unsigned version)
{
	ar.template serializeBase<MSXDevice>(*this);

	ar.serialize("reference", reference);
	ar.serialize("status", status);
	ar.serialize("DValue", DValue);
	ar.serialize("hold", hold);

	hardwareMute(!(status & 0x02));  // restore hwMute
	// TODO write dac??
}
INSTANTIATE_SERIALIZE_METHODS(MSXTurboRPCM);

} // namespace openmsx

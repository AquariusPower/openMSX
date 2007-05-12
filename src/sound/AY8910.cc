// $Id$

/*
 * Emulation of the AY-3-8910
 *
 * Original code taken from xmame-0.37b16.1
 *   Based on various code snippets by Ville Hallik, Michael Cuddy,
 *   Tatsuyuki Satoh, Fabrice Frances, Nicola Salmoria.
 * Integrated into openMSX by ???.
 * Refactored in C++ style by Maarten ter Huurne.
 */

#include "AY8910.hh"
#include "AY8910Periphery.hh"
#include "MSXMotherBoard.hh"
#include "MSXCliComm.hh"
#include "SimpleDebuggable.hh"
#include "XMLElement.hh"
#include "MSXException.hh"
#include "StringOp.hh"
#include <cassert>
#include <cstring>

using std::string;

namespace openmsx {

class AY8910Debuggable : public SimpleDebuggable
{
public:
	AY8910Debuggable(MSXMotherBoard& motherBoard, AY8910& ay8910);
	virtual byte read(unsigned address, const EmuTime& time);
	virtual void write(unsigned address, byte value, const EmuTime& time);
private:
	AY8910& ay8910;
};


// Fixed point representation of 1.
static const int FP_UNIT = 0x8000;

static const int CLOCK = 3579545 / 2;
static const int PORT_A_DIRECTION = 0x40;
static const int PORT_B_DIRECTION = 0x80;
enum Register {
	AY_AFINE = 0, AY_ACOARSE = 1, AY_BFINE = 2, AY_BCOARSE = 3,
	AY_CFINE = 4, AY_CCOARSE = 5, AY_NOISEPER = 6, AY_ENABLE = 7,
	AY_AVOL = 8, AY_BVOL = 9, AY_CVOL = 10, AY_EFINE = 11,
	AY_ECOARSE = 12, AY_ESHAPE = 13, AY_PORTA = 14, AY_PORTB = 15
};


// Generator:

AY8910::Generator::Generator()
{
	reset();
}

inline void AY8910::Generator::reset(byte output)
{
	period = 0;
	count = 0;
	this->output = output;
}

inline void AY8910::Generator::setPeriod(int value, unsigned updateStep)
{
	// A note about the period of tones, noise and envelope: for speed
	// reasons, we count down from the period to 0, but careful studies of the
	// chip output prove that it instead counts up from 0 until the counter
	// becomes greater or equal to the period. This is an important difference
	// when the program is rapidly changing the period to modulate the sound.
	// To compensate for the difference, when the period is changed we adjust
	// our internal counter.
	// Also, note that period = 0 is the same as period = 1. This is mentioned
	// in the YM2203 data sheets. However, this does NOT apply to the Envelope
	// period. In that case, period = 0 is half as period = 1.
	const int old = period;
	if (value == 0) value = 1;
	period = value * updateStep;
	count += period - old;
	if (count <= 0) count = 1;
}


// ToneGenerator:

template <bool enabled>
inline int AY8910::ToneGenerator::advance(int duration)
{
	int highDuration = 0;
	if (enabled && output) highDuration += count;
	count -= duration;
	if (count <= 0) {
		// Calculate number of output transitions.
		int cycles = 1 + (-count) / period;
		if (enabled) {
			// Full square waves: output is 1 half of the time;
			// which half doesn't matter.
			highDuration += period * (cycles / 2);
		}
		if (cycles & 1) {
			// Half a square wave.
			output ^= 1;
			if (enabled && output) highDuration += period;
		}
		count += period * cycles;
	}
	if (enabled && output) highDuration -= count;
	return highDuration;
}


// NoiseGenerator:

AY8910::NoiseGenerator::NoiseGenerator()
{
	reset();
}

inline void AY8910::NoiseGenerator::reset()
{
	Generator::reset(0x38);
	random = 1;
}

inline byte AY8910::NoiseGenerator::getOutput()
{
	return output;
}

inline int AY8910::NoiseGenerator::advanceToFlip(int duration)
{
	int left = duration;
	while (true) {
		if (count > left) {
			// Exit: end of interval.
			count -= left;
			return duration;
		}
		left -= count;
		count = period;
		// Is noise output going to change?
		const bool flip = (random + 1) & 2; // bit0 ^ bit1
		// The Random Number Generator of the 8910 is a 17-bit shift register.
		// The input to the shift register is bit0 XOR bit2 (bit0 is the
		// output).
		// The following is a fast way to compute bit 17 = bit0^bit2.
		// Instead of doing all the logic operations, we only check bit 0,
		// relying on the fact that after two shifts of the register, what now
		// is bit 2 will become bit 0, and will invert, if necessary, bit 16,
		// which previously was bit 18.
		// Note: On Pentium 4, the "if" causes trouble in the pipeline.
		//       After all this is pseudo-random and therefore a nightmare
		//       for branch prediction.
		//       A bit more calculation without a branch is faster.
		//       Without the "if", the transformation described above still
		//       speeds up the code, because the same "random & N"
		//       subexpression appears twice (also when doing multiple cycles
		//       in one go, see "advance" method).
		//       TODO: Benchmark on other modern CPUs.
		//if (random & 1) random ^= 0x28000;
		//random >>= 1;
		random =  (random >> 1)
		       ^ ((random & 1) << 14)
		       ^ ((random & 1) << 16);
		if (flip) {
			// Exit: output flip.
			output ^= 0x38;
			return duration - left;
		}
	}
}

inline void AY8910::NoiseGenerator::advance(int duration)
{
	int cycles = (duration + period - count) / period;
	count += cycles * period - duration;
	// See advanceToFlip for explanation of noise algorithm.
	for (; cycles >= 4405; cycles -= 4405) {
		random ^= (random >> 10)
		       ^ ((random & 0x003FF) << 5)
		       ^ ((random & 0x003FF) << 7);
	}
	for (; cycles >= 291; cycles -= 291) {
		random ^= (random >> 6)
		       ^ ((random & 0x3F) << 9)
		       ^ ((random & 0x3F) << 11);
	}
	for (; cycles >= 15; cycles -= 15) {
		random =  (random & 0x07FFF)
		       ^  (random >> 15)
		       ^ ((random & 0x07FFF) << 2);
	}
	while (cycles--) {
		random =  (random >> 1)
		       ^ ((random & 1) << 14)
		       ^ ((random & 1) << 16);
	}
	output = (random & 1) ? 0x38 : 0x00;
}


// Amplitude:

AY8910::Amplitude::Amplitude(const XMLElement& config)
{
	string type = StringOp::toLower(config.getChildData("type", "ay8910"));
	if (type == "ay8910") {
		ay8910 = true;
	} else if (type == "ym2149") {
		ay8910 = false;
	} else {
		throw FatalError("Unknown PSG type: " + type);
	}

	vol[0] = vol[1] = vol[2] = 0;
	envChan[0] = false;
	envChan[1] = false;
	envChan[2] = false;
	envVolume = 0;
	setMasterVolume(32768);
}

inline unsigned AY8910::Amplitude::getVolume(byte chan)
{
	return vol[chan];
}

inline void AY8910::Amplitude::setChannelVolume(byte chan, byte value)
{
	envChan[chan] = value & 0x10;
	vol[chan] = envChan[chan] ? envVolume : volTable[value & 0x0F];
}

inline void AY8910::Amplitude::setEnvelopeVolume(byte volume)
{
	envVolume = envVolTable[volume];
	if (envChan[0]) vol[0] = envVolume;
	if (envChan[1]) vol[1] = envVolume;
	if (envChan[2]) vol[2] = envVolume;
}

inline void AY8910::Amplitude::setMasterVolume(int volume)
{
	// Calculate the volume->voltage conversion table.
	// The AY-3-8910 has 16 levels, in a logarithmic scale (3dB per step).
	// YM2149 has 32 levels, the 16 extra levels are only used for envelope
	// volumes

	double out = volume; // avoid clipping
	for (int i = 31; i > 0; --i) {
		envVolTable[i] = (unsigned)(out + 0.5); // round to nearest;
		out *= 0.841395141645195;               // 1/(10^(1.5/20)) = 1/(1.5dB)
	}
	envVolTable[0] = 0;
	volTable[0] = 0;
	for (int i = 1; i < 16; ++i) {
		volTable[i] = envVolTable[2 * i + 1];
	}
	if (ay8910) {
		// only 16 envelope steps, duplicate every step
		envVolTable[1] = 0;
		for (int i = 2; i < 32; i += 2) {
			envVolTable[i] = envVolTable[i + 1];
		}
	}
}

inline bool AY8910::Amplitude::anyEnvelope()
{
	return envChan[0] || envChan[1] || envChan[2];
}


// Envelope:

// AY8910 and YM2149 behave different here:
//  YM2149 envelope goes twice as fast and has twice as many levels. Here
//  we implement the YM2149 behaviour, but to get the AY8910 behaviour we
//  repeat every level twice in the envVolTable

inline AY8910::Envelope::Envelope(Amplitude& amplitude)
	: amplitude(amplitude)
{
	period = 0;
	count  = 0;
	step   = 0;
	attack = 0;
	hold      = false;
	alternate = false;
	holding   = false;
}

inline void AY8910::Envelope::reset()
{
	period = 0;
	count = 0;
}

inline void AY8910::Envelope::setPeriod(int value, unsigned updateStep)
{
	// twice as fast as AY8910
	const int old = period;
	if (value == 0) {
		period = updateStep / 2;
	} else {
		period = value * updateStep;
	}
	count += period - old;
	if (count <= 0) count = 1;
}

inline byte AY8910::Envelope::getVolume()
{
	return step ^ attack;
}

inline void AY8910::Envelope::setShape(byte shape)
{
	// do 32 steps for both AY8910 and YM2149
	/*
	envelope shapes:
		C AtAlH
		0 0 x x  \___
		0 1 x x  /___
		1 0 0 0  \\\\
		1 0 0 1  \___
		1 0 1 0  \/\/
		1 0 1 1  \
		1 1 0 0  ////
		1 1 0 1  /
		1 1 1 0  /\/\
		1 1 1 1  /___
	*/
	attack = (shape & 0x04) ? 0x1F : 0x00;
	if ((shape & 0x08) == 0) {
		// If Continue = 0, map the shape to the equivalent one
		// which has Continue = 1.
		hold = true;
		alternate = attack;
	} else {
		hold = shape & 0x01;
		alternate = shape & 0x02;
	}
	count = period;
	step = 0x1F;
	holding = false;
	amplitude.setEnvelopeVolume(getVolume());
}

inline bool AY8910::Envelope::isChanging()
{
	return !holding;
}

inline void AY8910::Envelope::advance(int duration)
{
	if (!holding) {
		count -= duration;
		if (count <= 0) {
			const int steps = 1 + (-count) / period;
			step -= steps;
			count += steps * period;

			// Check current envelope position.
			if (step < 0) {
				if (hold) {
					if (alternate) attack ^= 0x1F;
					holding = true;
					step = 0;
				} else {
					// If step has looped an odd number of times
					// (usually 1), invert the output.
					if (alternate && (step & 0x10)) {
						attack ^= 0x1F;
					}
					step &= 0x1F;
				}
			}
			amplitude.setEnvelopeVolume(getVolume());
		}
	}
}


// AY8910 main class:

AY8910::AY8910(MSXMotherBoard& motherBoard, AY8910Periphery& periphery_,
               const XMLElement& config, const EmuTime& time)
	: SoundDevice(motherBoard.getMSXMixer(), "PSG", "PSG", 3)
	, cliComm(motherBoard.getMSXCliComm())
	, periphery(periphery_)
	, debuggable(new AY8910Debuggable(motherBoard, *this))
	, amplitude(config)
	, envelope(amplitude)
	, warningPrinted(false)
{
	// make valgrind happy
	memset(regs, 0, sizeof(regs));
	setOutputRate(44100);

	reset(time);
	registerSound(config);
}

AY8910::~AY8910()
{
	unregisterSound();
}

void AY8910::reset(const EmuTime& time)
{
	// Reset generators and envelope.
	for (byte chan = 0; chan < 3; chan++) {
		tone[chan].reset();
	}
	noise.reset();
	envelope.reset();
	// Reset registers and values derived from them.
	oldEnable = 0;
	for (byte reg = 0; reg <= 15; reg++) {
		wrtReg(reg, 0, time);
	}
}


byte AY8910::readRegister(byte reg, const EmuTime& time)
{
	assert(reg <= 15);
	switch (reg) {
	case AY_PORTA:
		if (!(regs[AY_ENABLE] & PORT_A_DIRECTION)) { //input
			regs[reg] = periphery.readA(time);
		}
		break;
	case AY_PORTB:
		if (!(regs[AY_ENABLE] & PORT_B_DIRECTION)) { //input
			regs[reg] = periphery.readB(time);
		}
		break;
	}
	return regs[reg];
}

byte AY8910::peekRegister(byte reg, const EmuTime& time) const
{
	assert(reg <= 15);
	switch (reg) {
	case AY_PORTA:
		if (!(regs[AY_ENABLE] & PORT_A_DIRECTION)) { //input
			return periphery.readA(time);
		}
		break;
	case AY_PORTB:
		if (!(regs[AY_ENABLE] & PORT_B_DIRECTION)) { //input
			return periphery.readB(time);
		}
		break;
	}
	return regs[reg];
}


void AY8910::writeRegister(byte reg, byte value, const EmuTime& time)
{
	assert(reg <= 15);
	if ((reg < AY_PORTA) && (reg == AY_ESHAPE || regs[reg] != value)) {
		// Update the output buffer before changing the register.
		updateStream(time);
	}
	wrtReg(reg, value, time);
}
void AY8910::wrtReg(byte reg, byte value, const EmuTime& time)
{
	// Warn/force port directions
	if (reg == AY_ENABLE) {
		if ((value & PORT_A_DIRECTION) && !warningPrinted) {
			warningPrinted = true;
			cliComm.printWarning(
				"The running MSX software has set unsafe PSG "
				"port directions (port A is set as output). "
				"This is not allowed by the MSX standard. "
				"Some MSX models (mostly MSX1) can get damaged "
				"by this.");
		}
		// portA -> input
		// portB -> output
		value = (value & ~PORT_A_DIRECTION) | PORT_B_DIRECTION;
	}

	// Note: unused bits are stored as well; they can be read back.
	regs[reg] = value;

	switch (reg) {
	case AY_AFINE:
	case AY_ACOARSE:
	case AY_BFINE:
	case AY_BCOARSE:
	case AY_CFINE:
	case AY_CCOARSE:
		tone[reg / 2].setPeriod(
			regs[reg & ~1] + 256 * (regs[reg | 1] & 0x0F), updateStep);
		break;
	case AY_NOISEPER:
		// half the frequency of tone generation
		noise.setPeriod(value & 0x1F, updateStep * 2);
		break;
	case AY_AVOL:
	case AY_BVOL:
	case AY_CVOL:
		amplitude.setChannelVolume(reg - AY_AVOL, value);
		break;
	case AY_EFINE:
	case AY_ECOARSE:
		// also half the frequency of tone generation, but handled
		// inside Envelope::setPeriod()
		envelope.setPeriod(
			regs[AY_EFINE] + 256 * regs[AY_ECOARSE], updateStep);
		break;
	case AY_ESHAPE:
		envelope.setShape(value);
		break;
	case AY_ENABLE:
		if ((value      & PORT_A_DIRECTION) &&
		    !(oldEnable & PORT_A_DIRECTION)) {
			// Changed from input to output.
			periphery.writeA(regs[AY_PORTA], time);
		}
		if ((value     & PORT_B_DIRECTION) &&
		    !(oldEnable & PORT_B_DIRECTION)) {
			// Changed from input to output.
			periphery.writeB(regs[AY_PORTB], time);
		}
		oldEnable = value;
		break;
	case AY_PORTA:
		if (regs[AY_ENABLE] & PORT_A_DIRECTION) { // output
			periphery.writeA(value, time);
		}
		break;
	case AY_PORTB:
		if (regs[AY_ENABLE] & PORT_B_DIRECTION) { // output
			periphery.writeB(value, time);
		}
		break;
	}
}

void AY8910::setOutputRate(unsigned sampleRate)
{
	// The step clock for the tone and noise generators is the chip clock
	// divided by 8; for the envelope generator of the AY-3-8910, it is half
	// that much (clock/16).
	// Here we calculate the number of steps which happen during one sample
	// at the given sample rate. No. of events = sample rate / (clock/8).
	// FP_UNIT is a multiplier used to turn the fraction into a fixed point
	// number.

	// !! look out for overflow !!
	updateStep = (FP_UNIT * sampleRate) / (CLOCK / 8);
	setInputRate(sampleRate);
}


void AY8910::generateChannels(int** bufs, unsigned length)
{
	/*
	static long long totalSamples = 0, noiseOff = 0, toneOff = 0, bothOff = 0;
	static long long envSamples = 0;
	static long long sameSample = 0;
	long long oldTotal = totalSamples;
	for (byte chan = 0; chan < 3; chan++) {
		totalSamples += length;
		if (regs[AY_ENABLE] & (0x08 << chan)) noiseOff += length;
		if (regs[AY_ENABLE] & (0x01 << chan)) toneOff += length;
		if ((regs[AY_ENABLE] & (0x09 << chan)) == (0x09 << chan)) {
			bothOff += length;
		}
	}
	if (amplitude.anyEnvelope()) envSamples += length;
	if ((totalSamples / 100000) != (oldTotal / 100000)) {
		fprintf(stderr,
			"%lld samples, %3.3f%% noise, %3.3f%% tone, %3.3f%% neither, "
			"%3.3f%% envelope, %3.3f%% same sample\n",
			totalSamples,
			100.0 * ((double)(totalSamples - noiseOff)/(double)totalSamples),
			100.0 * ((double)(totalSamples - toneOff)/(double)totalSamples),
			100.0 * ((double)bothOff/(double)totalSamples),
			100.0 * ((double)envSamples/(double)totalSamples),
			100.0 * ((double)sameSample/(double)totalSamples)
			);
	}
	*/

	if (((regs[AY_AVOL] | regs[AY_BVOL] | regs[AY_CVOL]) & 0x1F) == 0) {
		// optimization: all channels volume 0
		for (int i = 0; i < 3; ++i) {
			tone[i].advance<false>(length * FP_UNIT);
			bufs[i] = 0;
		}
		noise.advance(length * FP_UNIT);
		envelope.advance(length * FP_UNIT);
		return;
	}

	byte chanEnable = regs[AY_ENABLE];
	// Disable channels with volume 0: since the sample value doesn't matter,
	// we can use the fastest path.
	for (byte chan = 0; chan < 3; chan++) {
		if ((regs[AY_AVOL + chan] & 0x1F) == 0) {
			chanEnable |= 0x09 << chan;
		}
	}
	// Advance tone generators for channels that have tone disabled.
	for (byte chan = 0; chan < 3; chan++) {
		if (chanEnable & (0x01 << chan)) { // disabled
			tone[chan].advance<false>(length * FP_UNIT);
		}
	}
	// Noise enabled on any channel?
	bool anyNoise = (chanEnable & 0x38) != 0x38;
	if (!anyNoise) {
		noise.advance(length * FP_UNIT);
	}
	// Envelope enabled on any channel?
	bool enveloping = amplitude.anyEnvelope() && envelope.isChanging();
	if (!enveloping) {
		envelope.advance(length * FP_UNIT);
	}

	// Calculate samples.
	for (unsigned i = 0; i < length; ++i) {
		// semiVol keeps track of how long each square wave stays
		// in the 1 position during the sample period.
		int semiVol[3] = { 0, 0, 0 };
		int left = FP_UNIT;
		do {
			// The 8910 has three outputs, each output is the mix of one of
			// the three tone generators and of the (single) noise generator.
			// The two are mixed BEFORE going into the DAC. The formula to mix
			// each channel is:
			//   (ToneOn | ToneDisable) & (NoiseOn | NoiseDisable),
			//   where ToneOn and NoiseOn are the current generator state
			//   and ToneDisable and NoiseDisable come from the enable reg.
			// Note that this means that if both tone and noise are disabled,
			// the output is 1, not 0, and can be modulated by changing the
			// volume.

			// Update state of noise generator.
			byte chanFlags;
			int nextEvent;
			if (anyNoise) {
				// Next event is end of this sample or noise flip.
				chanFlags = noise.getOutput() | chanEnable;
				nextEvent = noise.advanceToFlip(left);
			} else {
				// Next event is end of this sample.
				chanFlags = chanEnable;
				nextEvent = FP_UNIT;
			}

			// Mix tone generators with noise generator.
			for (byte chan = 0; chan < 3; chan++, chanFlags >>= 1) {
				if ((chanFlags & 0x09) == 0x08) {
					// Square wave: alternating between 0 and 1.
					semiVol[chan] += tone[chan].advance<true>(nextEvent);
				} else if ((chanFlags & 0x09) == 0x09) {
					// Channel disabled: always 1.
					semiVol[chan] += nextEvent;
				} else if ((chanFlags & 0x09) == 0x00) {
					// Tone enabled, but suppressed by noise state.
					tone[chan].advance<false>(nextEvent);
				} else { // (chanFlags & 0x09) == 0x01
					// Tone disabled, noise state is 0.
					// Nothing to do.
				}
			}

			left -= nextEvent;
		} while (left > 0);

		// Update envelope.
		if (enveloping) envelope.advance(FP_UNIT);

		// Calculate D/A converter output.
		// TODO: Is it easy to detect when multiple samples have the same
		//       value? (make nextEvent depend on tone events as well?)
		//       At 44KHz, value typically changes once every 4 to 5 samples.
		bufs[0][i] = (semiVol[0] * amplitude.getVolume(0)) / FP_UNIT;
		bufs[1][i] = (semiVol[1] * amplitude.getVolume(1)) / FP_UNIT;
		bufs[2][i] = (semiVol[2] * amplitude.getVolume(2)) / FP_UNIT;
	}
}


// SimpleDebuggable

AY8910Debuggable::AY8910Debuggable(MSXMotherBoard& motherBoard, AY8910& ay8910_)
	: SimpleDebuggable(motherBoard, ay8910_.getName() + " regs",
	                   "PSG", 0x10)
	, ay8910(ay8910_)
{
}

byte AY8910Debuggable::read(unsigned address, const EmuTime& time)
{
	return ay8910.readRegister(address, time);
}

void AY8910Debuggable::write(unsigned address, byte value, const EmuTime& time)
{
	return ay8910.writeRegister(address, value, time);
}

} // namespace openmsx

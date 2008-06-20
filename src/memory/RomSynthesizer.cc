// $Id$

/* On Sat, 3 Apr 2004, Manuel Pazos wrote:
 *
 * As you know, the cartridge has an 8bit D/A, accessed through
 * memory-mapped at address #4000 by the program. OpenMSX also uses that
 * address to access it. But examining the cartridge board I found that the
 * address is decoded by a LS138 in this way:
 *
 *    /WR = L
 *    A15 = L
 *    A4 = L
 *    /MERQ = L
 *    /SLT = L
 *    A14 = H
 *
 * So any value equal to %01xxxxxxxxx0xxxx should work (i.e.: #4000, #4020,
 * #7C00, etc.)
*/

#include "RomSynthesizer.hh"
#include "MSXMotherBoard.hh"
#include "DACSound8U.hh"
#include "CacheLine.hh"
#include "Rom.hh"

namespace openmsx {

RomSynthesizer::RomSynthesizer(
		MSXMotherBoard& motherBoard, const XMLElement& config,
		std::auto_ptr<Rom> rom)
	: Rom16kBBlocks(motherBoard, config, rom)
{
	setBank(0, unmappedRead);
	setRom (1, 0);
	setRom (2, 1);
	setBank(3, unmappedRead);

	dac.reset(new DACSound8U(motherBoard.getMSXMixer(), "Synthesizer-DAC",
	                         "Konami Synthesizer's DAC", config));

	reset(getCurrentTime());
}

RomSynthesizer::~RomSynthesizer()
{
}

void RomSynthesizer::reset(const EmuTime& time)
{
	dac->reset(time);
}

void RomSynthesizer::writeMem(word address, byte value, const EmuTime& time)
{
	if ((address & 0xC010) == 0x4000) {
		dac->writeDAC(value, time);
	}
}

byte* RomSynthesizer::getWriteCacheLine(word address) const
{
	if ((address & 0xC010 & CacheLine::HIGH) == 0x4000) {
		return NULL;
	} else {
		return unmappedWrite;
	}
}

} // namespace openmsx

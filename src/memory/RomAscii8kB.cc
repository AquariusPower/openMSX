// $Id$

// ASCII 8kB cartridges
//
// this type is used in many japanese-only cartridges.
// example of cartridges: Valis(Fantasm Soldier), Dragon Slayer, Outrun,
//                        Ashguine 2, ...
// The address to change banks:
//  bank 1: 0x6000 - 0x67ff (0x6000 used)
//  bank 2: 0x6800 - 0x6fff (0x6800 used)
//  bank 3: 0x7000 - 0x77ff (0x7000 used)
//  bank 4: 0x7800 - 0x7fff (0x7800 used)

#include "RomAscii8kB.hh"
#include "Rom.hh"
#include "serialize.hh"

namespace openmsx {

RomAscii8kB::RomAscii8kB(MSXMotherBoard& motherBoard, const XMLElement& config,
                         std::auto_ptr<Rom> rom)
	: Rom8kBBlocks(motherBoard, config, rom)
{
	reset(EmuTime::dummy());
}

void RomAscii8kB::reset(EmuTime::param /*time*/)
{
	setBank(0, unmappedRead);
	setBank(1, unmappedRead);
	for (int i = 2; i < 6; i++) {
		setRom(i, 0);
	}
	setBank(6, unmappedRead);
	setBank(7, unmappedRead);
}

void RomAscii8kB::writeMem(word address, byte value, EmuTime::param /*time*/)
{
	if ((0x6000 <= address) && (address < 0x8000)) {
		byte region = ((address >> 11) & 3) + 2;
		setRom(region, value);
	}
}

byte* RomAscii8kB::getWriteCacheLine(word address) const
{
	if ((0x6000 <= address) && (address < 0x8000)) {
		return NULL;
	} else {
		return unmappedWrite;
	}
}

REGISTER_MSXDEVICE(RomAscii8kB, "RomAscii8kB");

} // namespace openmsx

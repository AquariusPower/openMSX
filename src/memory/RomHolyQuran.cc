// $Id$

// Holy Qu'ran  cartridge
//  It is like an ASCII 8KB, but using the 5000h, 5400h, 5800h and 5C00h
//  addresses.


#include "RomHolyQuran.hh"
#include "Rom.hh"
#include "serialize.hh"

namespace openmsx {

RomHolyQuran::RomHolyQuran(
		MSXMotherBoard& motherBoard, const XMLElement& config,
		std::auto_ptr<Rom> rom)
	: Rom8kBBlocks(motherBoard, config, rom)
{
	reset(EmuTime::dummy());
}

void RomHolyQuran::reset(EmuTime::param /*time*/)
{
	setBank(0, unmappedRead);
	setBank(1, unmappedRead);
	for (int i = 2; i < 6; i++) {
		setRom(i, 0);
	}
	setBank(6, unmappedRead);
	setBank(7, unmappedRead);
}

void RomHolyQuran::writeMem(word address, byte value, EmuTime::param /*time*/)
{
	// TODO are switch addresses mirrored?
	if ((0x5000 <= address) && (address < 0x6000)) {
		byte region = ((address >> 10) & 3) + 2;
		setRom(region, value);
	}
}

byte* RomHolyQuran::getWriteCacheLine(word address) const
{
	if ((0x5000 <= address) && (address < 0x6000)) {
		return NULL;
	} else {
		return unmappedWrite;
	}
}

REGISTER_MSXDEVICE(RomHolyQuran, "RomHolyQuran");

} // namespace openmsx

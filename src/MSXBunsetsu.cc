// $Id$

#include "MSXBunsetsu.hh"
#include "CacheLine.hh"
#include "Rom.hh"

namespace openmsx {

MSXBunsetsu::MSXBunsetsu(MSXMotherBoard& motherBoard, const XMLElement& config,
                         const EmuTime& time)
	: MSXDevice(motherBoard, config, time)
	, bunsetsuRom(new Rom(motherBoard, getName() + "_1", "rom",
	                      config, "bunsetsu"))
	, jisyoRom   (new Rom(motherBoard, getName() + "_2", "rom",
	                      config, "jisyo"))
{
	reset(time);
}

MSXBunsetsu::~MSXBunsetsu()
{
}

void MSXBunsetsu::reset(const EmuTime& /*time*/)
{
	jisyoAddress = 0;
}

byte MSXBunsetsu::readMem(word address, const EmuTime& /*time*/)
{
	byte result;
	if (address == 0xBFFF) {
		result = (*jisyoRom)[jisyoAddress];
		jisyoAddress = (jisyoAddress + 1) & 0x1FFFF;
	} else if ((0x4000 <= address) && (address < 0xC000)) {
		result = (*bunsetsuRom)[address - 0x4000];
	} else {
		result = 0xFF;
	}
	return result;
}

void MSXBunsetsu::writeMem(word address, byte value, const EmuTime& /*time*/)
{
	switch (address) {
		case 0xBFFC:
			jisyoAddress = (jisyoAddress & 0x1FF00) | value;
			break;
		case 0xBFFD:
			jisyoAddress = (jisyoAddress & 0x100FF) | (value << 8);
			break;
		case 0xBFFE:
			jisyoAddress = (jisyoAddress & 0x0FFFF) |
			               ((value & 1) << 16);
			break;
	}
}

const byte* MSXBunsetsu::getReadCacheLine(word start) const
{
	if ((start & CacheLine::HIGH) == (0xBFFF & CacheLine::HIGH)) {
		return NULL;
	} else {
		return &(*bunsetsuRom)[start - 0x4000];
	}
}

byte* MSXBunsetsu::getWriteCacheLine(word start) const
{
	if ((start & CacheLine::HIGH) == (0xBFFF & CacheLine::HIGH)) {
		return NULL;
	} else {
		return unmappedWrite;
	}
}

} // namespace openmsx

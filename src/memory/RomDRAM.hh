// $Id$

#ifndef ROMDRAM_HH
#define ROMDRAM_HH

#include "MSXRom.hh"

namespace openmsx {

class PanasonicMemory;

class RomDRAM : public MSXRom
{
public:
	RomDRAM(MSXMotherBoard& motherBoard, const XMLElement& config,
	        std::auto_ptr<Rom> rom);

	virtual byte readMem(word address, const EmuTime& time);
	virtual const byte* getReadCacheLine(word start) const;

private:
	PanasonicMemory& panasonicMemory;
	unsigned baseAddr;
};

} // namespace openmsx

#endif

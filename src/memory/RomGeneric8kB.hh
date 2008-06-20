// $Id$

#ifndef ROMGENERIC8KB_HH
#define ROMGENERIC8KB_HH

#include "Rom8kBBlocks.hh"

namespace openmsx {

class RomGeneric8kB : public Rom8kBBlocks
{
public:
	RomGeneric8kB(MSXMotherBoard& motherBoard, const XMLElement& config,
	              std::auto_ptr<Rom> rom);

	virtual void reset(const EmuTime& time);
	virtual void writeMem(word address, byte value, const EmuTime& time);
	virtual byte* getWriteCacheLine(word address) const;
};

} // namespace openmsx

#endif

// $Id$

#ifndef ROMGENERIC16KB_HH
#define ROMGENERIC16KB_HH

#include "Rom16kBBlocks.hh"

namespace openmsx {

class RomGeneric16kB : public Rom16kBBlocks
{
public:
	RomGeneric16kB(MSXMotherBoard& motherBoard, const XMLElement& config,
	               std::auto_ptr<Rom> rom);

	virtual void reset(const EmuTime& time);
	virtual void writeMem(word address, byte value, const EmuTime& time);
	virtual byte* getWriteCacheLine(word address) const;
};

} // namespace openmsx

#endif
